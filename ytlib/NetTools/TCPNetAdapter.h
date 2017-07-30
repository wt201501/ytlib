#pragma once
#include <ytlib/SupportTools/Serialize.h>
#include <ytlib/Common/FileSystem.h>
#include <ytlib/NetTools/TcpNetUtil.h>
#include <ytlib/SupportTools/QueueBase.h>
#include <boost/thread.hpp>
#include <future>
#include <memory>
#include <shared_mutex>
#include <atomic>

namespace ytlib {
	/*
	ʹ��boost.asio��һ����������������
	ʹ��ʱ��Ҫ����һ������id����������Ϣ��map��Ȼ����ʱֻ�贫�����ݺ�Ŀ�ĵ�id����

	ʹ��boost���л��ķ�ʽֱ�ӽ������л�����������ֱ�ӽ������������л����ࡣ	�������Ա�⣬��֧�֣�
	1�������ݿ�����map_datas��
	2��һЩ�����ڶ��С�ȷ�������ڲ���ֻ�������ݣ�����ȡ������ָ�����map_datas��
	3���ļ����͡���Ҫȷ�����͡�����Ŀ¼������ֱ�ӷŵ���ִ���ļ�Ŀ¼��

	���ǿ�ƽ̨�ԣ�ǣ�浽���紫�������һ��ʹ�� char����׼int���֣�������wchar��int��long��size_t��Щ����ȷ�ģ�
	-------------------------------------------------------
	�������ݶ�ʹ��shared_buf����ʽ�洢���ṩһЩ���صĺ�����һЩ����������ʽת���shared_buf������������չ������ӣ�
	�ṩ���ִ�����������ݵķ�ʽ��
	1��safe��ʽ��������ʵ�����ݿ�����һ��shared_buf
	2��unsafe��ʽ��ֻ֧�ִ���shared_buf���û���Ҫ��֤���ݲ���������ɾ�����޸�
	(ʵ��������ֻ����һ��ʾ������һЩ�Ķ�����������ֱ��ʹ�������ķ�ʽ������������ǵþ���д���ݽṹ���ﵽ���Ч��)
	----------------------------------------------------
	sock���ӳ�,	���ַ�ʽ�������ӣ�1��accep����	2���������ӵ�
	���ص����Ӷ��󶨵�һ��port��
	*/

	struct shared_buf {
		boost::shared_array<char> buf;
		uint32_t buf_size;
	};

	class LightSignal {
	public:
		LightSignal() :flag(false) {}
		~LightSignal() {}
		void notify_one() {
			std::lock_guard<std::mutex> lck(m_mutex);
			flag = true;
			m_cond.notify_one();
		}
		void wait() {
			std::unique_lock<std::mutex> lck(m_mutex);
			if (flag) return;
			m_cond.wait(lck);
		}
	private:
		std::mutex m_mutex;
		std::condition_variable m_cond;
		bool flag;
	};

	//T��Ҫ��boost���л�
	template<class T>
	class DataPackage {
	public:
		T obj;//�����л�����
		//tip-data��ʽ
		std::map<std::string, shared_buf> map_datas;//����,���֧��255��
		std::map<std::string, std::string> map_files;//�ļ�,���֧��255��
		bool complete_flag;//�������flag
		LightSignal* p_s;
	};

	typedef boost::asio::ip::tcp::endpoint TcpEp;//28���ֽ�
	template<class T>
	class TcpNetAdapter {
		typedef std::shared_ptr<DataPackage<T>> dataPtr;
	private:
		/*
		tag: 2 byte
			c+0:����
			d+255:�������ݵ�tip����\n�ָ�
			d+i:��i������
			f+255:�����ļ���tip+�ļ�����ֻ���ļ�����û��·��������ʽ��tip1=filename1\ntip2=filename2\n...
			f+i:��i���ļ�������
			o+v:������
		*/
		enum {
			CLASSHEAD = 'C',
			DATAHEAD = 'D',
			FILEHEAD = 'F',
		};
		static const uint8_t HEAD_SIZE = 8;

		//���ࣺsock���ӣ�Ϊ��������������׼����ֻ����������ͬ��д����ȡ���첽�Զ���
		class TcpConnection :boost::noncopyable {
			typedef std::shared_ptr<boost::asio::streambuf> buff_Ptr;
		public:
			TcpConnection(boost::asio::io_service& io_, 
				std::function<void(const TcpEp &)> errcb_,
				tpath* p_RecvPath_,
				std::function<void(dataPtr &)> cb_):
				sock(io_),
				err_CallBack(errcb_), 
				stopflag(false),
				m_recv_callback(cb_), 
				p_RecvPath(p_RecvPath_),
				m_DataQueue(2000),
				m_queueThread(std::bind(&TcpConnection::run_queue,this)){
				
			}
			virtual ~TcpConnection() { 
				stopflag = true; 
				m_DataQueue.Stop();
				m_queueThread.join();
			}

			//������������
			bool connect(const TcpEp& ep_, uint16_t port_ = 0) {
				sock.open(boost::asio::ip::tcp::v4());
				if (port_) {
					sock.set_option(boost::asio::ip::tcp::socket::reuse_address(true));
					sock.bind(TcpEp(boost::asio::ip::tcp::v4(), port_));//���ָ���˿��ˣ���󶨵�ָ���˿�
				}
				boost::system::error_code err;
				sock.connect(ep_, err);
				if (err) {
					YT_DEBUG_PRINTF("connect failed : %s\n", err.message().c_str());
					return false;
				}
				remote_ep = ep_;
				return true;
			}
			//ͬ�����ͣ�����
			bool write(const std::shared_ptr<std::vector<boost::asio::const_buffer>> & data_) {
				boost::system::error_code err;
				write_mutex.lock();
				sock.write_some(*data_, err);
				write_mutex.unlock();
				if (err) {
					if (stopflag) return false;
					YT_DEBUG_PRINTF("write failed : %s\n", err.message().c_str());
					err_CallBack(remote_ep);
					return false;
				}
				return true;
			}
			
			inline void start() {do_read_head();}

			boost::asio::ip::tcp::socket sock;
			TcpEp remote_ep;
		private:
			bool read_get_err(const boost::system::error_code & err) {
				if (stopflag) return true;
				if (err) {
					YT_DEBUG_PRINTF("read failed : %s\n", err.message().c_str());
					err_CallBack(remote_ep);
					return true;
				}
				return false;
			}
			inline void do_read_head(dataPtr& d_= dataPtr()) {
				boost::asio::async_read(sock, boost::asio::buffer(header, HEAD_SIZE), boost::asio::transfer_exactly(HEAD_SIZE),
					std::bind(&TcpConnection::on_read_head, this, d_, std::placeholders::_1, std::placeholders::_2));
			}

			void run_queue() {
				dataPtr p;
				while (!stopflag) {
					if (!m_DataQueue.BlockDequeue(p)) return;
					LightSignal* p_s = p->p_s;
					p.reset();
					p_s->wait();
					delete p_s;
				}
			}
			//ʹ������ָ���ɾ�������ж������Ƿ�׼���ò����лص�
			void on_data_ready(DataPackage<T> * p) {
				LightSignal* p_s = p->p_s;
				if (p->complete_flag) {
					m_recv_callback(std::move(dataPtr(p)));//��ִ����ص���׼����һ��
					p_s->notify_one();
					return;
				}
				p_s->notify_one();
				delete p;
			}
			//������Żص���
			void on_read_head(dataPtr& RData_,const boost::system::error_code & err, size_t read_bytes) {
				if (read_get_err(err)) return;
				//������ͷ
				if (header[0] == TCPHEAD1 && header[1] == TCPHEAD2 && read_bytes == HEAD_SIZE) {
					uint32_t pack_size = get_num_from_buf(&header[4]);
					if (!RData_) {
						if (header[2] == CLASSHEAD) {
							//�½�һ�����ݰ�
							RData_ = dataPtr((new DataPackage<T>()), std::bind(&TcpConnection::on_data_ready, this, std::placeholders::_1));
							RData_->complete_flag = false;
							RData_->p_s = new LightSignal();
							m_DataQueue.Enqueue(RData_);
							buff_Ptr pBuff = buff_Ptr(new boost::asio::streambuf());
							boost::asio::async_read(sock, *pBuff, boost::asio::transfer_exactly(pack_size),
								std::bind(&TcpConnection::on_read_obj, this, RData_, pBuff, std::placeholders::_1, std::placeholders::_2));
							return;
						}
					}
					else {
						if (header[2] == DATAHEAD) {
							boost::shared_array<char> pDataBuff = boost::shared_array<char>(new char[pack_size]);
							if (header[3] == static_cast<char>(uint8_t(255))) {
								boost::asio::async_read(sock, boost::asio::buffer(pDataBuff.get(), pack_size), boost::asio::transfer_exactly(pack_size),
									std::bind(&TcpConnection::on_read_data_tips, this, RData_, pDataBuff, std::placeholders::_1, std::placeholders::_2));
							}
							else {
								boost::asio::async_read(sock, boost::asio::buffer(pDataBuff.get(), pack_size), boost::asio::transfer_exactly(pack_size),
									std::bind(&TcpConnection::on_read_data, this, RData_, pDataBuff, std::placeholders::_1, std::placeholders::_2));
							}
							return;
						}
						if (header[2] == FILEHEAD) {
							boost::shared_array<char> pDataBuff = boost::shared_array<char>(new char[pack_size]);
							if (header[3] == static_cast<char>(uint8_t(255))) {
								boost::asio::async_read(sock, boost::asio::buffer(pDataBuff.get(), pack_size), boost::asio::transfer_exactly(pack_size),
									std::bind(&TcpConnection::on_read_file_tips, this, RData_, pDataBuff, std::placeholders::_1, std::placeholders::_2));
							}
							else {
								boost::asio::async_read(sock, boost::asio::buffer(pDataBuff.get(), pack_size), boost::asio::transfer_exactly(pack_size),
									std::bind(&TcpConnection::on_read_file, this, RData_, pDataBuff, std::placeholders::_1, std::placeholders::_2));
							}
							return;
						}
						if (header[2] == TCPEND1 && header[3] == TCPEND2) {
							do_read_head();
							RData_->complete_flag = true;
							return;
						}
					}
				}

				YT_DEBUG_PRINTF("read failed : recv an invalid header : %c %c %c %d %d\n",
					header[0], header[1], header[2], header[3], get_num_from_buf(&header[4]));
				err_CallBack(remote_ep);
				return;
			}
			void on_read_obj(dataPtr& RData_, buff_Ptr& buff_,const boost::system::error_code & err, size_t read_bytes) {
				if (read_get_err(err)) return;
				do_read_head(RData_);
				boost::archive::binary_iarchive iar(*buff_);
				iar >> RData_->obj;
			}
			void on_read_data_tips(dataPtr& RData_, boost::shared_array<char>& buff_, const boost::system::error_code & err, size_t read_bytes) {
				if (read_get_err(err)) return;
				//һ��Ҫ�Ƚ�����map��do_read_head
				uint32_t pos = 0;
				for (uint32_t ii = 0; ii < read_bytes; ++ii) {
					if (buff_[ii] == '\n') {
						RData_->map_datas.insert(std::pair<std::string, shared_buf>(string(&buff_[pos], ii - pos), shared_buf()));
						pos = ii + 1;
					}
				}
				do_read_head(RData_);
			}
			void on_read_data(dataPtr& RData_, boost::shared_array<char>& buff_, const boost::system::error_code & err, size_t read_bytes) {
				if (read_get_err(err)) return;
				uint8_t pos = header[3];
				if (pos > RData_->map_datas.size()) {
					YT_DEBUG_PRINTF("read failed : recv an invalid data");
					err_CallBack(remote_ep);
					return;
				}
				do_read_head(RData_);
				std::map<std::string, shared_buf>::iterator itr = RData_->map_datas.begin();
				for (; pos > 0; --pos) ++itr;
				itr->second.buf = buff_;
				itr->second.buf_size = static_cast<uint32_t>(read_bytes);
			}
			void on_read_file_tips(dataPtr& RData_, boost::shared_array<char>& buff_, const boost::system::error_code & err, size_t read_bytes) {
				if (read_get_err(err)) return;
				//һ��Ҫ�Ƚ�����map��do_read_head
				uint32_t pos = 0, pos1 = 0;
				for (uint32_t ii = 0; ii < read_bytes; ++ii) {
					if (buff_[ii] == '=') {
						pos1 = ii + 1;
					}
					if (buff_[ii] == '\n') {
						RData_->map_files.insert(std::pair<std::string, std::string>(
							string(&buff_[pos], pos1 - 1 - pos), string(&buff_[pos1], ii - pos1)));
						pos = ii + 1;
					}
				}
				do_read_head(RData_);
			}
			void on_read_file(dataPtr& RData_, boost::shared_array<char>& buff_, const boost::system::error_code & err, size_t read_bytes) {
				if (read_get_err(err)) return;
				uint8_t pos = header[3];
				if (pos > RData_->map_files.size()) {
					YT_DEBUG_PRINTF("read failed : recv an invalid file");
					err_CallBack(remote_ep);
					return;
				}
				do_read_head(RData_);
				std::map<std::string, std::string>::iterator itr = RData_->map_files.begin();
				for (; pos > 0 ; --pos) ++itr;
				std::ofstream f(((*p_RecvPath) / tpath(T_STRING_TO_TSTRING(itr->second))).string<tstring>(), ios::out | ios::trunc | ios::binary);
				if(f){
					f.write(buff_.get(), read_bytes);
					f.close();
				}
				else {
					YT_DEBUG_PRINTF("can not write file : %s", ((*p_RecvPath) / tpath(T_STRING_TO_TSTRING(itr->second))).string<std::string>().c_str());
				}
				
			}

			std::atomic_bool stopflag;
			std::function<void(const TcpEp &)> err_CallBack;//��������ʱ�Ļص���һ����/д�����͹ر����Ӳ����ûص���֪�ϲ�
			std::mutex write_mutex;
			std::function<void(dataPtr &)> m_recv_callback;
			tpath* p_RecvPath;//�����ļ�·��
			char header[HEAD_SIZE];//���ջ���

			QueueBase<dataPtr> m_DataQueue;
			std::thread m_queueThread;
		};
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		
	public:
		TcpNetAdapter(uint32_t myid_,
			const TcpEp & hostInfo_,
			std::function<void(dataPtr &)> recvcb_,
			const tstring& rp = T_TEXT(""),
			const tstring& sp = T_TEXT(""),
			uint32_t threadSize_ = 10) :
			m_myid(myid_), 
			m_receiveCallBack(recvcb_),
			m_RecvPath(tGetAbsolutePath(rp)), 
			m_SendPath(tGetAbsolutePath(sp)),
			myport(hostInfo_.port()),
			stopflag(true),
			m_threadSize(threadSize_){
			m_mapHostInfo[myid_] = hostInfo_;
			boost::filesystem::create_directories(m_RecvPath);
			boost::filesystem::create_directories(m_SendPath);
		}
		virtual ~TcpNetAdapter(){
			stopflag = true;
			service.stop();
			acceptorPtr.reset();
			m_mapTcpConnection.swap(std::map<TcpEp, TcpConnectionPtr>());
			m_RunThreads.join_all();
		}

		inline bool start() {
			if (!checkPort(myport)) return false;
			service.reset();
			stopflag = false;
			acceptorPtr = std::shared_ptr<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor(service, TcpEp(boost::asio::ip::tcp::v4(), myport), true));
			TcpConnectionPtr pConnection = getNewTcpConnectionPtr();
			acceptorPtr->async_accept(pConnection->sock, std::bind(&TcpNetAdapter::on_accept, this, pConnection, std::placeholders::_1));
			//ʹ�����10���߳�
			for (uint32_t i = 0; i < m_threadSize; ++i) {
				m_RunThreads.create_thread(boost::bind(&boost::asio::io_service::run, &service));
			}
			return true;
		}

		bool Send(const dataPtr & Tdata_, const std::vector<uint32_t>& dst_) {
			//��һЩ���
			if (stopflag) return false;
			size_t size = dst_.size();
			if (size == 0) return false;
			if (Tdata_->map_datas.size() > 255 || Tdata_->map_files.size() > 255) return false;//���֧��255�����ݰ�/�ļ�
			std::vector<TcpEp> vec_hosts;
			std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
			for (size_t ii = 0; ii < size; ++ii) {
				if (dst_[ii] == m_myid) return false;
				std::map<uint32_t, TcpEp>::const_iterator itr = m_mapHostInfo.find(dst_[ii]);
				if (itr == m_mapHostInfo.end()) return false;
				vec_hosts.push_back(itr->second);
			}
			lck.unlock();

			//��Tdata_��ת��Ϊstd::vector<boost::asio::const_buffer>��һ���Է���
			std::shared_ptr<std::vector<boost::asio::const_buffer>> buffersPtr =
				std::shared_ptr<std::vector<boost::asio::const_buffer>>(new std::vector<boost::asio::const_buffer>());
			//��һ�������Ͷ���
			char c_head_buff[HEAD_SIZE]{ TCPHEAD1 ,TCPHEAD2,CLASSHEAD,0 };
			boost::asio::streambuf objbuff;
			boost::archive::binary_oarchive oar(objbuff);
			oar << Tdata_->obj;
			set_buf_from_num(&c_head_buff[4], static_cast<uint32_t>(objbuff.size()));
			buffersPtr->push_back(std::move(boost::asio::const_buffer(c_head_buff, HEAD_SIZE)));
			buffersPtr->push_back(std::move(objbuff.data()));
			
			//��2���������ļ�,�ȷ����ļ�����Ϣ
			std::map<std::string, std::string>& map_files = Tdata_->map_files;
			std::string file_tips;
			char f0_head_buff[HEAD_SIZE]{ TCPHEAD1 ,TCPHEAD2,FILEHEAD,static_cast<char>(uint8_t(255)) };
			boost::shared_array<char> f_head_buff;
			std::vector<boost::shared_array<char>> vec_file_buf;
			if (map_files.size() > 0) {
				f_head_buff = boost::shared_array<char>(new char[map_files.size() * HEAD_SIZE]);
				for (std::map<std::string, std::string>::const_iterator itr = map_files.begin(); itr != map_files.end(); ++itr) {
					file_tips += itr->first; file_tips += '=';//tip
					file_tips += boost::filesystem::path(itr->second).filename().string(); file_tips += '\n';
				}
				set_buf_from_num(&f0_head_buff[4], static_cast<uint32_t>(file_tips.size()));
				buffersPtr->push_back(std::move(boost::asio::const_buffer(f0_head_buff, HEAD_SIZE)));
				buffersPtr->push_back(std::move(boost::asio::buffer(file_tips)));
				//�ٷ���ÿ���ļ��������Ʒ�ʽ���ļ������ڵĻ����������ļ�
				uint8_t ii = 0;
				for (std::map<std::string, std::string>::const_iterator itr = map_files.begin(); itr != map_files.end(); ++itr) {
					tpath file_path(T_STRING_TO_TSTRING(itr->second));
					if (!file_path.is_absolute()) {
						file_path = m_SendPath / file_path;
					}
					std::ifstream f(file_path.string<tstring>(), ios::in | ios::binary);
					if (f) {
						size_t cur_offerset = ii * HEAD_SIZE;
						memcpy(&f_head_buff[cur_offerset], f0_head_buff, 3);
						f_head_buff[cur_offerset + 3] = static_cast<char>(ii);
						f.seekg(0, f.end);
						uint32_t length = static_cast<uint32_t>(f.tellg());
						set_buf_from_num(&f_head_buff[cur_offerset + 4], length);
						f.seekg(0, f.beg);
						boost::shared_array<char> file_buf = boost::shared_array<char>(new char[length]);
						f.read(file_buf.get(), length);
						f.close();
						buffersPtr->push_back(std::move(boost::asio::const_buffer(&f_head_buff[cur_offerset], HEAD_SIZE)));
						buffersPtr->push_back(std::move(boost::asio::const_buffer(file_buf.get(), length)));
						vec_file_buf.push_back(std::move(file_buf));
					}
					++ii;
				}
			}

			//��3������������,�ȷ���tips���ݰ�
			std::map<std::string, shared_buf>& map_datas = Tdata_->map_datas;
			//���������ܷ����ڲ�scope��
			std::string data_tips;
			char d0_head_buff[HEAD_SIZE]{ TCPHEAD1 ,TCPHEAD2,DATAHEAD,static_cast<char>(uint8_t(255)) };
			boost::shared_array<char> d_head_buff;
			if (map_datas.size() > 0) {
				d_head_buff = boost::shared_array<char>(new char[map_datas.size() * HEAD_SIZE]);
				for (std::map<std::string, shared_buf>::const_iterator itr = map_datas.begin(); itr != map_datas.end(); ++itr) {
					data_tips += itr->first;
					data_tips += '\n';
				}
				set_buf_from_num(&d0_head_buff[4], static_cast<uint32_t>(data_tips.size()));
				buffersPtr->push_back(std::move(boost::asio::const_buffer(d0_head_buff, HEAD_SIZE)));
				buffersPtr->push_back(std::move(boost::asio::buffer(data_tips)));
				//�ٷ���ÿ�����ݰ���sizeΪ0�򲻷���
				uint8_t ii = 0;
				for (std::map<std::string, shared_buf>::const_iterator itr = map_datas.begin(); itr != map_datas.end(); ++itr) {
					if (itr->second.buf_size > 0) {
						size_t cur_offerset = ii * HEAD_SIZE;
						memcpy(&d_head_buff[cur_offerset], d0_head_buff, 3);
						d_head_buff[cur_offerset + 3] = static_cast<char>(ii);
						set_buf_from_num(&d_head_buff[cur_offerset + 4], itr->second.buf_size);
						buffersPtr->push_back(std::move(boost::asio::const_buffer(&d_head_buff[cur_offerset], HEAD_SIZE)));
						buffersPtr->push_back(std::move(boost::asio::const_buffer(itr->second.buf.get(), itr->second.buf_size)));
					}
					++ii;
				}
			}

			//���Ĳ������ͽ�����
			char end_head_buff[HEAD_SIZE]{ TCPHEAD1 ,TCPHEAD2,TCPEND1,TCPEND2 };
			buffersPtr->push_back(std::move(boost::asio::const_buffer(end_head_buff, HEAD_SIZE)));

			//����ַ�Ż�
			if (vec_hosts.size() == 1) {
				return _send_one(buffersPtr, vec_hosts[0]);
			}
			//����ʹ�ö��̲߳���Ⱥ��
			std::vector<std::future<bool>> v;
			size_t len = vec_hosts.size();
			for (size_t ii = 0; ii < len; ii++) {
				v.push_back(std::async(std::launch::async, &TcpNetAdapter::_send_one, this, buffersPtr, vec_hosts[ii]));
			}
			bool result = true;
			for (size_t ii = 0; ii < len; ii++) {
				result = result && (v[ii].get());
			}
			return result;
		}

		//hostinfo��������Ҫ�Ƕ����ṩ��ֻ����ӻ��޸ģ������Ƴ�
		inline TcpEp GetMyHostInfo() {
			std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
			return m_mapHostInfo[m_myid];
		}
		inline std::map<uint32_t, TcpEp> GetHostInfoMap() {
			std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
			return m_mapHostInfo;
		}
		//��������info�����򸲸ǣ��������
		bool SetHost(uint32_t hostid_, const TcpEp & hostInfo_) {
			std::unique_lock<std::shared_mutex> lck(m_hostInfoMutex);
			m_mapHostInfo[hostid_] = hostInfo_;
			return true;
		}
		bool SetHost(const std::map<uint32_t, TcpEp>& hosts_) {
			std::unique_lock<std::shared_mutex> lck(m_hostInfoMutex);
			for (std::map<uint32_t, TcpEp>::iterator itr = hosts_.begin(); itr != hosts_.end(); ++itr) {
				m_mapHostInfo[itr->first] = itr->second;
			}
			return true;
		}

	private:

		//��������
		inline TcpConnectionPtr getNewTcpConnectionPtr() {
			return TcpConnectionPtr(new TcpConnection(service, std::bind(&TcpNetAdapter::on_err, this, std::placeholders::_1), &m_RecvPath, m_receiveCallBack));
		}
		void on_accept(TcpConnectionPtr& p, const boost::system::error_code & err) {
			if (stopflag) return;
			if (err) {
				YT_DEBUG_PRINTF("listerner get err, please restart : %s\n", err.message().c_str());
				return;
			}
			p->remote_ep = p->sock.remote_endpoint();
			YT_DEBUG_PRINTF("get a new connection from %s:%d\n", p->remote_ep.address().to_string().c_str(), p->remote_ep.port());
			m_TcpConnectionMutex.lock();
			m_mapTcpConnection[p->remote_ep] = p;
			m_TcpConnectionMutex.unlock();
			p->start();
			TcpConnectionPtr pConnection = getNewTcpConnectionPtr();
			acceptorPtr->async_accept(pConnection->sock, std::bind(&TcpNetAdapter::on_accept, this, pConnection, std::placeholders::_1));

		}
		bool _send_one(const std::shared_ptr<std::vector<boost::asio::const_buffer>> & Tdata_, const TcpEp & ep) {
			std::shared_ptr<TcpConnection> pc = getTcpConnectionPtr(ep);
			if (!pc) return false;
			return pc->write(Tdata_);
		}

		//��ȡ����
		TcpConnectionPtr getTcpConnectionPtr(const TcpEp& ep) {
			//����map���ң�û�о�ֱ��ȥ����һ��
			{
				std::shared_lock<std::shared_mutex> lck(m_TcpConnectionMutex);
				std::map<TcpEp, TcpConnectionPtr>::iterator itr = m_mapTcpConnection.find(ep);
				if (itr != m_mapTcpConnection.end()) {
					return itr->second;
				}
			}
			//ͬ������
			TcpConnectionPtr pConnection = getNewTcpConnectionPtr();
			if (pConnection->connect(ep, myport)) {
				YT_DEBUG_PRINTF("connect to %s:%d successful\n", ep.address().to_string().c_str(), ep.port());
				m_TcpConnectionMutex.lock();
				m_mapTcpConnection[ep] = pConnection;
				m_TcpConnectionMutex.unlock();
				pConnection->start();
				return pConnection;
			}
			else {
				//���ͬ������ʧ���ˣ�Ҳ�п��ܶԷ��Ѿ����ӹ����ˡ��������ڱ�����һ��
				std::shared_lock<std::shared_mutex> lck(m_TcpConnectionMutex);
				std::map<TcpEp, TcpConnectionPtr>::iterator itr = m_mapTcpConnection.find(ep);
				if (itr != m_mapTcpConnection.end()) {
					return itr->second;
				}
			}
			YT_DEBUG_PRINTF("connect to %s:%d failed\n", ep.address().to_string().c_str(), ep.port());
			return TcpConnectionPtr();
		}

		void on_err(const TcpEp& ep) {
			YT_DEBUG_PRINTF("connection to %s:%d get an err and is closed\n", ep.address().to_string().c_str(), ep.port());
			std::unique_lock<std::shared_mutex> lck(m_TcpConnectionMutex);
			std::map<TcpEp, TcpConnectionPtr>::iterator itr = m_mapTcpConnection.find(ep);
			if (itr != m_mapTcpConnection.end()) {
				m_mapTcpConnection.erase(itr);
			}
		}

		std::atomic_bool stopflag;//ֹͣ��־
		std::function<void(dataPtr &)> m_receiveCallBack;//���ջص�
		tpath m_RecvPath;//�����ļ�·��
		tpath m_SendPath;//�����ļ�·��

		boost::thread_group m_RunThreads;

		std::map<TcpEp, TcpConnectionPtr> m_mapTcpConnection;//Ŀ��ep-TcpConnection��map
		std::shared_mutex m_TcpConnectionMutex;

		std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptorPtr;//������
		boost::asio::io_service service;

		std::map<uint32_t, TcpEp> m_mapHostInfo;//�����б�id-info
		std::shared_mutex m_hostInfoMutex;//�����б�Ķ�д��

		const uint32_t m_threadSize;//ʹ�õ��첽�߳�����
		const uint32_t m_myid;//����id������֮���޷��޸�
		const uint16_t myport;//�����˿ڣ����������������е����Ӷ��󶨵�����˿���
	};

}