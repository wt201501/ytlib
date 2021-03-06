/**
 * @file TcpNetAdapter.h
 * @brief TCP网络适配器
 * @details 使用boost.asio，基于ConnPool的简易网络适配器
 * @author WT
 * @date 2019-07-26
 */
#pragma once

#include "FileSystem.h"
#include "serialize.hpp"
#include "SharedBuf.h"
#include "ConnPool.h"

#include "ytlib/thread_tools/block_queue.hpp"
#include "ytlib/thread_tools/signal.hpp"

#include <atomic>
#include <future>
#include <memory>

namespace ytlib {
/*
  使用boost.asio的一个简易网络适配器
  使用时需要建立一个主机id与其网络信息的map，然后发送时只需传递数据和目的地id即可

  使用boost序列化的方式直接将类序列化到发送流、直接将接收流反序列化到类。  除了类成员外，还支持：
  1、将数据拷贝进map_datas中
  2、一些创建在堆中、确保生存期并且只读的数据，可以取其智能指针放于map_datas中
  3、文件发送。需要确立发送、接收目录，否则直接放到可执行文件目录下

  考虑跨平台性，牵涉到网络传输的数据一律使用 char、标准int这种（不能用wchar、int、long、size_t这些不明确的）
  -------------------------------------------------------
  所有数据都使用sharedBuf的形式存储。提供一些重载的函数将一些常见数据形式转变成sharedBuf。（还可以拓展自行添加）
  提供两种存入待发送数据的方式：
  1、safe方式：将数据实际内容拷贝到一个sharedBuf
  2、unsafe方式：只支持传入sharedBuf。用户需要保证数据不被其主动删除或修改
  (实际上这里只是做一个示例，仅一些改动不大的情况会直接使用这样的方式。具体情况还是得具体写数据结构来达到最高效率)
  ----------------------------------------------------
  sock连接池,  两种方式建立连接：1、accep到的  2、主动连接的
  本地的连接都绑定到一个port上
  -----------------------------------------------------
  tag: 2 byte
  c+0:对象
  q+0:快速内容
  d+255:所有数据的tip，以\n分割
  d+i:第i个数据
  f+255:所有文件的tip+文件名（只有文件名，没有路径），格式：tip1=filename1\ntip2=filename2\n...
  f+i:第i个文件的数据
  o+v:结束符
 */

/**
 * @brief 基础数据包类型
 * T需要能boost序列化
 */
template <class T>
class DataPackage {
 public:
  T obj;                 //可序列化的类
  sharedBuf quick_data;  //快速内容
  //tip-data形式
  std::map<std::string, sharedBuf> map_datas;    //数据,最大支持255个
  std::map<std::string, std::string> map_files;  //文件,最大支持255个
};

/**
 * @brief tcp连接类
 * 为网络适配器定制准备，只能主动发起同步写，读取是异步自动的
 */
template <class T>
class TcpConnection : public ConnBase {
  typedef std::shared_ptr<boost::asio::streambuf> buff_Ptr;
  typedef std::shared_ptr<DataPackage<T>> dataPtr;
  struct RecvDataPackage {
    dataPtr pdata;
    bool complete_flag;  ///<接收完成flag
    LightSignal* p_s;
  };
  typedef std::shared_ptr<RecvDataPackage> RecvDataPtr;

 public:
  enum {
    CLASSHEAD = 'C',
    QUICKHEAD = 'Q',
    DATAHEAD = 'D',
    FILEHEAD = 'F'
  };
  TcpConnection(boost::asio::io_service& io_,
                std::function<void(const TcpEp&)> errcb_,
                tpath const* p_RecvPath_,
                std::function<void(dataPtr&)> cb_) : ConnBase(io_, errcb_),
                                                     m_recv_callback(cb_),
                                                     p_RecvPath(p_RecvPath_),
                                                     m_DataQueue(2000),
                                                     m_queueThread(std::bind(&TcpConnection::run_queue, this)) {
  }
  virtual ~TcpConnection() {
    stopflag = true;
    m_DataQueue.Stop();
    m_queueThread.join();
  }

  ///用于主动连接
  bool connect(const TcpEp& ep_, uint16_t port_ = 0) {
    sock_.open(boost::asio::ip::tcp::v4());
    boost::system::error_code err;
    if (port_) {
      //如果指定端口了，则绑定到指定端口
      sock_.set_option(TcpSocket::reuse_address(true));
      sock_.bind(TcpEp(boost::asio::ip::tcp::v4(), port_), err);
      /*
      if (err) {
        YT_DEBUG_PRINTF("connect failed : %s", err.message().c_str());
        return false;
      }
      */
    }

    sock_.connect(ep_, err);
    if (err) {
      YT_DEBUG_PRINTF("connect failed : %s", err.message().c_str());
      return false;
    }
    remote_ep = ep_;
    return true;
  }
  ///同步发送，加锁
  bool write(const std::shared_ptr<std::vector<boost::asio::const_buffer>>& data_) {
    boost::system::error_code err;
    write_mutex.lock();
    sock_.write_some(*data_, err);
    write_mutex.unlock();
    if (err) {
      if (stopflag) return false;
      YT_DEBUG_PRINTF("write failed : %s", err.message().c_str());
      errcb_(remote_ep);
      return false;
    }
    return true;
  }
  void start() {
    RecvDataPtr d_ = RecvDataPtr();
    do_read_head(d_);
  }

 private:
  void do_read_head(RecvDataPtr& d_) {
    boost::asio::async_read(sock_, boost::asio::buffer(header, HEAD_SIZE), boost::asio::transfer_exactly(HEAD_SIZE),
                            std::bind(&TcpConnection::on_read_head, this, d_, std::placeholders::_1, std::placeholders::_2));
  }
  void run_queue() {
    RecvDataPtr p;
    while (!stopflag) {
      if (!m_DataQueue.BlockDequeue(p)) return;
      LightSignal* p_s = p->p_s;
      p.reset();
      p_s->wait();
      delete p_s;
    }
  }
  ///使用智能指针的删除器来判断数据是否准备好并进行回调
  void on_data_ready(RecvDataPackage* p) {
    LightSignal* p_s = p->p_s;
    if (p->complete_flag) {
      m_recv_callback(p->pdata);  //先执行完回调再准备下一个
    }
    p_s->notify();
    delete p;
  }
  ///读取解析报头，缓存跟着回调走
  void on_read_head(RecvDataPtr& RData_, const boost::system::error_code& err, std::size_t read_bytes) {
    if (read_get_err(err)) return;
    if (header[0] == TCPHEAD1 && header[1] == TCPHEAD2 && read_bytes == HEAD_SIZE) {
      uint32_t pack_size = GetNumFromBuf(&header[4]);
      if (!RData_) {
        if (header[2] == CLASSHEAD) {
          //新建一个数据包
          RData_ = RecvDataPtr((new RecvDataPackage()), std::bind(&TcpConnection::on_data_ready, this, std::placeholders::_1));
          RData_->pdata = std::make_shared<DataPackage<T>>();
          RData_->complete_flag = false;
          RData_->p_s = new LightSignal();
          m_DataQueue.Enqueue(RData_);
          buff_Ptr pBuff = std::make_shared<boost::asio::streambuf>();
          boost::asio::async_read(sock_, *pBuff, boost::asio::transfer_exactly(pack_size),
                                  std::bind(&TcpConnection::on_read_obj, this, RData_, pBuff, std::placeholders::_1, std::placeholders::_2));
          return;
        }
      } else {
        if (header[2] == QUICKHEAD) {
          boost::shared_array<char> pDataBuff = boost::shared_array<char>(new char[pack_size]);
          boost::asio::async_read(sock_, boost::asio::buffer(pDataBuff.get(), pack_size), boost::asio::transfer_exactly(pack_size),
                                  std::bind(&TcpConnection::on_read_quick_data, this, RData_, pDataBuff, std::placeholders::_1, std::placeholders::_2));
          return;
        }
        if (header[2] == FILEHEAD) {
          boost::shared_array<char> pDataBuff = boost::shared_array<char>(new char[pack_size]);
          if (header[3] == static_cast<char>(uint8_t(255))) {
            boost::asio::async_read(sock_, boost::asio::buffer(pDataBuff.get(), pack_size), boost::asio::transfer_exactly(pack_size),
                                    std::bind(&TcpConnection::on_read_file_tips, this, RData_, pDataBuff, std::placeholders::_1, std::placeholders::_2));
          } else {
            boost::asio::async_read(sock_, boost::asio::buffer(pDataBuff.get(), pack_size), boost::asio::transfer_exactly(pack_size),
                                    std::bind(&TcpConnection::on_read_file, this, RData_, pDataBuff, std::placeholders::_1, std::placeholders::_2));
          }
          return;
        }
        if (header[2] == DATAHEAD) {
          boost::shared_array<char> pDataBuff = boost::shared_array<char>(new char[pack_size]);
          if (header[3] == static_cast<char>(uint8_t(255))) {
            boost::asio::async_read(sock_, boost::asio::buffer(pDataBuff.get(), pack_size), boost::asio::transfer_exactly(pack_size),
                                    std::bind(&TcpConnection::on_read_data_tips, this, RData_, pDataBuff, std::placeholders::_1, std::placeholders::_2));
          } else {
            boost::asio::async_read(sock_, boost::asio::buffer(pDataBuff.get(), pack_size), boost::asio::transfer_exactly(pack_size),
                                    std::bind(&TcpConnection::on_read_data, this, RData_, pDataBuff, std::placeholders::_1, std::placeholders::_2));
          }
          return;
        }
        if (header[2] == TCPEND1 && header[3] == TCPEND2) {
          RecvDataPtr d_ = RecvDataPtr();
          do_read_head(d_);
          RData_->complete_flag = true;
          return;
        }
      }
    }
    stopflag = true;
    YT_DEBUG_PRINTF("read failed : recv an invalid header : %c %c %c %d %d", header[0], header[1], header[2], header[3], GetNumFromBuf(&header[4]));
    errcb_(remote_ep);
    return;
  }
  void on_read_obj(RecvDataPtr& RData_, buff_Ptr& buff_, const boost::system::error_code& err, std::size_t read_bytes) {
    if (read_get_err(err)) return;
    do_read_head(RData_);
    boost::archive::binary_iarchive iar(*buff_);
    iar >> RData_->pdata->obj;
  }
  void on_read_quick_data(RecvDataPtr& RData_, boost::shared_array<char>& buff_, const boost::system::error_code& err, std::size_t read_bytes) {
    if (read_get_err(err)) return;
    do_read_head(RData_);
    RData_->pdata->quick_data.buf = buff_;
    RData_->pdata->quick_data.buf_size = (uint32_t)read_bytes;
  }
  void on_read_data_tips(RecvDataPtr& RData_, boost::shared_array<char>& buff_, const boost::system::error_code& err, std::size_t read_bytes) {
    if (read_get_err(err)) return;
    //一定要先建立好map再do_read_head
    uint32_t pos = 0;
    for (uint32_t ii = 0; ii < read_bytes; ++ii) {
      if (buff_[ii] == '\n') {
        RData_->pdata->map_datas.insert(std::pair<std::string, sharedBuf>(std::string(&buff_[pos], ii - pos), sharedBuf()));
        pos = ii + 1;
      }
    }
    do_read_head(RData_);
  }
  void on_read_data(RecvDataPtr& RData_, boost::shared_array<char>& buff_, const boost::system::error_code& err, std::size_t read_bytes) {
    if (read_get_err(err)) return;
    uint8_t pos = header[3];
    if (pos > RData_->pdata->map_datas.size()) {
      YT_DEBUG_PRINTF("read failed : recv an invalid data");
      errcb_(remote_ep);
      return;
    }
    do_read_head(RData_);
    std::map<std::string, sharedBuf>::iterator itr = RData_->pdata->map_datas.begin();
    for (; pos > 0; --pos) ++itr;
    itr->second.buf = buff_;
    itr->second.buf_size = static_cast<uint32_t>(read_bytes);
  }
  void on_read_file_tips(RecvDataPtr& RData_, boost::shared_array<char>& buff_, const boost::system::error_code& err, std::size_t read_bytes) {
    if (read_get_err(err)) return;
    //一定要先建立好map再do_read_head
    uint32_t pos = 0, pos1 = 0;
    for (uint32_t ii = 0; ii < read_bytes; ++ii) {
      if (buff_[ii] == '=') {
        pos1 = ii + 1;
      }
      if (buff_[ii] == '\n') {
        RData_->pdata->map_files.insert(std::pair<std::string, std::string>(
            std::string(&buff_[pos], pos1 - 1 - pos), std::string(&buff_[pos1], ii - pos1)));
        pos = ii + 1;
      }
    }
    do_read_head(RData_);
  }
  void on_read_file(RecvDataPtr& RData_, boost::shared_array<char>& buff_, const boost::system::error_code& err, std::size_t read_bytes) {
    if (read_get_err(err)) return;
    uint8_t pos = header[3];
    if (pos > RData_->pdata->map_files.size()) {
      YT_DEBUG_PRINTF("read failed : recv an invalid file");
      errcb_(remote_ep);
      return;
    }
    do_read_head(RData_);
    std::map<std::string, std::string>::iterator itr = RData_->pdata->map_files.begin();
    for (; pos > 0; --pos) ++itr;
    std::ofstream f(((*p_RecvPath) / tpath(T_STR_TO_TSTR(itr->second))).string<tstring>(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (f) {
      f.write(buff_.get(), read_bytes);
      f.close();
    } else {
      YT_DEBUG_PRINTF("can not write file : %s", ((*p_RecvPath) / tpath(T_STR_TO_TSTR(itr->second))).string<std::string>().c_str());
    }
  }

  std::mutex write_mutex;
  std::function<void(dataPtr&)> m_recv_callback;
  tpath const* p_RecvPath;  ///<接收文件路径

  BlockQueue<RecvDataPtr> m_DataQueue;
  std::thread m_queueThread;
};

/**
 * @brief tcp网络适配器
 * 默认使用uint32_t作为id形式。也可以改为std::string之类的可以作为map容器的key的类型
 */
template <class T, class IDType = uint32_t>
class TcpNetAdapter : public ConnPool<TcpConnection<T>> {
 private:
  typedef ConnPool<TcpConnection<T>> BaseClass;
  typedef std::shared_ptr<TcpConnection<T>> TcpConnectionPtr;
  typedef std::shared_ptr<DataPackage<T>> dataPtr;
  static const uint8_t HEAD_SIZE = TcpConnection<T>::HEAD_SIZE;

 public:
  TcpNetAdapter(IDType myid_,
                uint16_t port_,
                std::function<void(dataPtr&)> recvcb_,
                const tstring& rp = T_TEXT(""),
                const tstring& sp = T_TEXT(""),
                uint32_t threadSize_ = 10) : ConnPool<TcpConnection<T>>(port_, threadSize_),
                                             m_myid(myid_),
                                             m_receiveCallBack(recvcb_),
                                             m_RecvPath(tGetAbsolutePath(rp)),
                                             m_SendPath(tGetAbsolutePath(sp)) {
    m_mapHostInfo[myid_] = TcpEp(boost::asio::ip::tcp::v4(), port_);
    boost::filesystem::create_directories(m_RecvPath);
    boost::filesystem::create_directories(m_SendPath);
  }
  virtual ~TcpNetAdapter() { BaseClass::stop(); }

  inline bool Send(const dataPtr& Tdata_, const IDType& dst_, bool delfiles = false) {
    return Send(Tdata_, std::vector<IDType>{dst_}, delfiles);
  }
  ///不重复发送的接口
  bool Send(const dataPtr& Tdata_, const std::set<IDType>& dst_, bool delfiles = false) {
    //做一些检查
    if (BaseClass::stopflag) return false;
    if (dst_.size() == 0) return false;
    if (Tdata_->map_datas.size() > 255 || Tdata_->map_files.size() > 255) return false;  //最大支持255个数据包/文件
    std::vector<TcpEp> vec_hosts;
    std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
    for (typename std::set<IDType>::const_iterator itr = dst_.begin(); itr != dst_.end(); ++itr) {
      if (*itr == m_myid) return false;  //不能发给自己。上层做好检查
      typename std::map<IDType, TcpEp>::const_iterator itr1 = m_mapHostInfo.find(*itr);
      if (itr1 == m_mapHostInfo.end()) return false;
      vec_hosts.push_back(itr1->second);
    }
    lck.unlock();
    return _Send(Tdata_, vec_hosts, delfiles);
  }

  ///使用vector作为地址容器，允许重复地址（重复地址意味着重复发送）
  bool Send(const dataPtr& Tdata_, const std::vector<IDType>& dst_, bool delfiles = false) {
    //做一些检查
    if (BaseClass::stopflag) return false;
    std::size_t size = dst_.size();
    if (size == 0) return false;
    if (Tdata_->map_datas.size() > 255 || Tdata_->map_files.size() > 255) return false;  //最大支持255个数据包/文件
    std::vector<TcpEp> vec_hosts;
    std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
    for (std::size_t ii = 0; ii < size; ++ii) {
      if (dst_[ii] == m_myid) return false;  //不能发给自己。上层做好检查
      typename std::map<IDType, TcpEp>::const_iterator itr = m_mapHostInfo.find(dst_[ii]);
      if (itr == m_mapHostInfo.end()) return false;
      vec_hosts.push_back(itr->second);
    }
    lck.unlock();
    return _Send(Tdata_, vec_hosts, delfiles);
  }

  ///hostinfo操作，主要是对外提供。只可添加或修改，不可移除
  inline TcpEp GetMyHostInfo() {
    std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
    return m_mapHostInfo[m_myid];
  }
  inline std::map<IDType, TcpEp> GetHostInfoMap() {
    std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
    return m_mapHostInfo;
  }
  ///设置主机info，有则覆盖，无责添加
  bool SetHost(const IDType& hostid_, const TcpEp& hostInfo_) {
    std::unique_lock<std::shared_mutex> lck(m_hostInfoMutex);
    m_mapHostInfo[hostid_] = hostInfo_;
    return true;
  }
  inline bool SetHost(const IDType& hostid_, const std::string& ip_, const uint16_t port_) {
    return SetHost(hostid_, TcpEp(boost::asio::ip::address::from_string(ip_), port_));
  }

  bool SetHost(std::map<IDType, TcpEp>& hosts_) {
    std::unique_lock<std::shared_mutex> lck(m_hostInfoMutex);
    m_mapHostInfo = hosts_;
    return true;
  }
  bool SetHost(const std::map<IDType, std::pair<std::string, uint16_t>>& hosts_) {
    std::unique_lock<std::shared_mutex> lck(m_hostInfoMutex);
    for (typename std::map<IDType, TcpEp>::iterator itr = hosts_.begin(); itr != hosts_.end(); ++itr) {
      m_mapHostInfo[itr->first] = std::move(TcpEp(boost::asio::ip::address::from_string(itr->second.first), itr->second.second));
    }
    return true;
  }

 private:
  TcpConnectionPtr GetNewTcpConnectionPtr() {
    return std::make_shared<TcpConnection<T>>(BaseClass::service, std::bind(&TcpNetAdapter::on_err, this, std::placeholders::_1), &m_RecvPath, m_receiveCallBack);
  }
  void on_err(const TcpEp& ep) { BaseClass::on_err(ep); }

  bool _send_one(const std::shared_ptr<std::vector<boost::asio::const_buffer>>& Tdata_, const TcpEp& ep) {
    std::shared_ptr<TcpConnection<T>> pc = getTcpConnectionPtr(ep);
    if (!pc) return false;
    return pc->write(Tdata_);
  }

  ///获取连接
  TcpConnectionPtr getTcpConnectionPtr(const TcpEp& ep) {
    //先在map里找，没有就直接去连接一个
    {
      std::shared_lock<std::shared_mutex> lck(BaseClass::tcp_conn_map_mutex_);
      typename std::map<TcpEp, TcpConnectionPtr>::iterator itr = BaseClass::tcp_conn_map_.find(ep);
      if (itr != BaseClass::tcp_conn_map_.end()) {
        return itr->second;
      }
    }
    //同步连接
    TcpConnectionPtr pConnection = GetNewTcpConnectionPtr();
    if (pConnection->connect(ep, BaseClass::port_)) {
      YT_DEBUG_PRINTF("connect to %s:%d successful", ep.address().to_string().c_str(), ep.port());
      BaseClass::tcp_conn_map_mutex_.lock();
      BaseClass::tcp_conn_map_[ep] = pConnection;
      BaseClass::tcp_conn_map_mutex_.unlock();
      pConnection->start();
      return pConnection;
    } else {
      //如果同步连接失败了，也有可能对方已经连接过来了。可以再在表中找一下
      std::shared_lock<std::shared_mutex> lck(BaseClass::tcp_conn_map_mutex_);
      typename std::map<TcpEp, TcpConnectionPtr>::iterator itr = BaseClass::tcp_conn_map_.find(ep);
      if (itr != BaseClass::tcp_conn_map_.end()) {
        return itr->second;
      }
    }
    YT_DEBUG_PRINTF("connect to %s:%d failed", ep.address().to_string().c_str(), ep.port());
    return TcpConnectionPtr();
  }

  bool _Send(const dataPtr& Tdata_, const std::vector<TcpEp>& vec_hosts, bool delfiles = false) {
    //将Tdata_先转换为std::vector<boost::asio::const_buffer>再一次性发送
    std::shared_ptr<std::vector<boost::asio::const_buffer>> buffersPtr = std::make_shared<std::vector<boost::asio::const_buffer>>();
    //第一步：发送对象
    char c_head_buff[HEAD_SIZE]{TcpConnection<T>::TCPHEAD1, TcpConnection<T>::TCPHEAD2, TcpConnection<T>::CLASSHEAD, 0};
    boost::asio::streambuf objbuff;
    boost::archive::binary_oarchive oar(objbuff);
    oar << Tdata_->obj;
    SetBufFromNum(&c_head_buff[4], static_cast<uint32_t>(objbuff.size()));
    buffersPtr->push_back(boost::asio::const_buffer(c_head_buff, HEAD_SIZE));
    buffersPtr->push_back(std::move(objbuff.data()));

    //第二步，发送快速数据
    char q_head_buff[HEAD_SIZE]{TcpConnection<T>::TCPHEAD1, TcpConnection<T>::TCPHEAD2, TcpConnection<T>::QUICKHEAD, 0};
    if (Tdata_->quick_data.buf_size > 0) {
      SetBufFromNum(&q_head_buff[4], Tdata_->quick_data.buf_size);
      buffersPtr->push_back(boost::asio::const_buffer(q_head_buff, HEAD_SIZE));
      buffersPtr->push_back(boost::asio::const_buffer(Tdata_->quick_data.buf.get(), Tdata_->quick_data.buf_size));
    }

    //第三步，发送文件,先发送文件名信息
    std::map<std::string, std::string>& map_files = Tdata_->map_files;
    std::string file_tips;
    char f0_head_buff[HEAD_SIZE]{TcpConnection<T>::TCPHEAD1, TcpConnection<T>::TCPHEAD2, TcpConnection<T>::FILEHEAD, static_cast<char>(uint8_t(255))};
    boost::shared_array<char> f_head_buff;
    std::vector<boost::shared_array<char>> vec_file_buf;
    if (map_files.size() > 0) {
      f_head_buff = boost::shared_array<char>(new char[map_files.size() * HEAD_SIZE]);
      for (std::map<std::string, std::string>::const_iterator itr = map_files.begin(); itr != map_files.end(); ++itr) {
        file_tips += itr->first;
        file_tips += '=';  //tip
        file_tips += boost::filesystem::path(itr->second).filename().string();
        file_tips += '\n';
      }
      SetBufFromNum(&f0_head_buff[4], static_cast<uint32_t>(file_tips.size()));
      buffersPtr->push_back(boost::asio::const_buffer(f0_head_buff, HEAD_SIZE));
      buffersPtr->push_back(boost::asio::const_buffer(file_tips.c_str(), file_tips.size()));
      //再发送每个文件。二进制方式。文件不存在的话就跳过该文件
      uint8_t ii = 0;
      for (std::map<std::string, std::string>::const_iterator itr = map_files.begin(); itr != map_files.end(); ++itr) {
        tpath file_path(T_STR_TO_TSTR(itr->second));
        if (!file_path.is_absolute()) {
          file_path = m_SendPath / file_path;
        }
        std::ifstream f(file_path.string<tstring>(), std::ios::in | std::ios::binary);
        if (f) {
          std::size_t cur_offerset = ii * HEAD_SIZE;
          memcpy(&f_head_buff[cur_offerset], f0_head_buff, 3);
          f_head_buff[cur_offerset + 3] = static_cast<char>(ii);
          f.seekg(0, f.end);
          uint32_t length = static_cast<uint32_t>(f.tellg());
          SetBufFromNum(&f_head_buff[cur_offerset + 4], length);
          f.seekg(0, f.beg);
          boost::shared_array<char> file_buf = boost::shared_array<char>(new char[length]);
          f.read(file_buf.get(), length);
          f.close();
          if (delfiles) {
            boost::system::error_code ec;
            boost::filesystem::remove(file_path, ec);
            if (ec) YT_DEBUG_PRINTF("delete file %s failed: %s", file_path.string().c_str(), ec.message().c_str());
          }
          buffersPtr->push_back(boost::asio::const_buffer(&f_head_buff[cur_offerset], HEAD_SIZE));
          buffersPtr->push_back(boost::asio::const_buffer(file_buf.get(), length));
          vec_file_buf.push_back(std::move(file_buf));
        }
        ++ii;
      }
    }

    //第四步，发送数据,先发送tips数据包
    std::map<std::string, sharedBuf>& map_datas = Tdata_->map_datas;
    //缓冲区不能放在内层scope里
    std::string data_tips;
    char d0_head_buff[HEAD_SIZE]{TcpConnection<T>::TCPHEAD1, TcpConnection<T>::TCPHEAD2, TcpConnection<T>::DATAHEAD, static_cast<char>(uint8_t(255))};
    boost::shared_array<char> d_head_buff;
    if (map_datas.size() > 0) {
      d_head_buff = boost::shared_array<char>(new char[map_datas.size() * HEAD_SIZE]);
      for (std::map<std::string, sharedBuf>::const_iterator itr = map_datas.begin(); itr != map_datas.end(); ++itr) {
        data_tips += itr->first;
        data_tips += '\n';
      }
      SetBufFromNum(&d0_head_buff[4], static_cast<uint32_t>(data_tips.size()));
      buffersPtr->push_back(boost::asio::const_buffer(d0_head_buff, HEAD_SIZE));
      buffersPtr->push_back(boost::asio::const_buffer(data_tips.c_str(), data_tips.size()));
      //再发送每个数据包。size为0则不发送
      uint8_t ii = 0;
      for (std::map<std::string, sharedBuf>::const_iterator itr = map_datas.begin(); itr != map_datas.end(); ++itr) {
        if (itr->second.buf_size > 0) {
          std::size_t cur_offerset = ii * HEAD_SIZE;
          memcpy(&d_head_buff[cur_offerset], d0_head_buff, 3);
          d_head_buff[cur_offerset + 3] = static_cast<char>(ii);
          SetBufFromNum(&d_head_buff[cur_offerset + 4], itr->second.buf_size);
          buffersPtr->push_back(boost::asio::const_buffer(&d_head_buff[cur_offerset], HEAD_SIZE));
          buffersPtr->push_back(boost::asio::const_buffer(itr->second.buf.get(), itr->second.buf_size));
        }
        ++ii;
      }
    }

    //第五步：发送结束符
    char end_head_buff[HEAD_SIZE]{TcpConnection<T>::TCPHEAD1, TcpConnection<T>::TCPHEAD2, TcpConnection<T>::TCPEND1, TcpConnection<T>::TCPEND2};
    buffersPtr->push_back(boost::asio::const_buffer(end_head_buff, HEAD_SIZE));

    //单地址优化
    if (vec_hosts.size() == 1) {
      return _send_one(buffersPtr, vec_hosts[0]);
    }
    //否则使用多线程并行群发
    std::vector<std::future<bool>> v;
    std::size_t len = vec_hosts.size();
    for (std::size_t ii = 0; ii < len; ii++) {
      v.push_back(std::async(std::launch::async, &TcpNetAdapter::_send_one, this, buffersPtr, vec_hosts[ii]));
    }
    bool result = true;
    for (std::size_t ii = 0; ii < len; ii++) {
      result = result && (v[ii].get());
    }
    return result;
  }

  std::function<void(dataPtr&)> m_receiveCallBack;  ///<接收回调
  tpath m_RecvPath;                                 ///<接收文件路径
  tpath m_SendPath;                                 ///<发送文件路径

  std::map<IDType, TcpEp> m_mapHostInfo;  ///<主机列表：id-info
  std::shared_mutex m_hostInfoMutex;       ///<主机列表的读写锁

  const IDType m_myid;  ///<自身id，构造之后无法修改
};

}  // namespace ytlib