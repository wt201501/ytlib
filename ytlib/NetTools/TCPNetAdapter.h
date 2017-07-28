#pragma once
#include <ytlib/NetTools/NetAdapterBase.h>
#include <ytlib/SupportTools/Serialize.h>
#include <ytlib/NetTools/TcpConnectionPool.h>
#include <ytlib/Common/FileSystem.h>
#include <boost/asio.hpp>
#include <future>

namespace ytlib {

	/*
	ʹ��boost.asio��һ����������������

	ʹ��boost���л��ķ�ʽֱ�ӽ������л���������
	ֱ�ӽ������������л�����


	�������Ա�⣬��֧�֣�
	1�������ݿ�����vec_datas��
	2��һЩ�����ڶ��С�ȷ�������ڲ���ֻ�������ݣ�����ȡ������ָ�����vec_datasPtr��
	3���ļ����͡���Ҫȷ�����͡�����Ŀ¼������ֱ�ӷŵ���ִ���ļ�Ŀ¼��

	���ǿ�ƽ̨�ԣ�ǣ�浽���紫�������һ��ʹ��:
	char
	��׼int��������int��long��size_t��Щ��


	ʵ��ʹ��ʱ��DataPackage�ټ�һ��ģ�壨��Ҫ�ü̳У�
	-------------------------------------------------------
	
	�������ݶ�ʹ��shared_buf����ʽ�洢���ṩһЩ���صĺ�����һЩ����������ʽת���shared_buf������������չ������ӣ�
	�ṩ���ִ�����������ݵķ�ʽ��
	1��safe��ʽ��������ʵ�����ݿ�����һ��shared_buf

	2��unsafe��ʽ��ֻ֧�ִ���shared_buf���û���Ҫ��֤���ݲ���������ɾ�����޸�
	
	(ʵ��������ֻ����һ��ʾ������һЩ�Ķ�����������ֱ��ʹ�������ķ�ʽ������������ǵþ���д���ݽṹ���ﵽ���Ч��)

	----------------------------------------------------
	����ʹ�����¸�ʽ��
	�ȴ�һ����ͷ����8 byte��
	head: 2 byte

	tag: 2 byte
	c+0:����
	d+255:�������ݵ�tip����\n�ָ�
	d+i:��i������
	f+255:�����ļ���tip+�ļ�����ֻ���ļ�����û��·��������ʽ��tip1=filename1\ntip2=filename2\n...
	f+i:��i���ļ�������
	o+v:������

	size: 4 byte ��Ĭ��windows����С��
	num = byte1+byte2*256+byte3*65536+byte4*2^24
	byte1=num%256,byte2=(num/256) ...

	Ȼ����size��byte������

	*/

	struct shared_buf {
		boost::shared_array<char> buf;
		uint32_t buf_size;
	};

	//T��Ҫ��boost���л�
	template<class T>
	class DataPackage {
	public:
		T obj;//�����л�����
		//tip-data��ʽ
		std::map<std::string, shared_buf> map_datas;//����,���֧��255��
		std::map<std::string, std::string> map_files;//�ļ�,���֧��255��
	};

	template<class T>
	class TcpNetAdapter : public NetAdapterBase<TcpEp, std::shared_ptr<DataPackage<T>>> {
	protected:
#define TCPHEAD1 'Y'
#define TCPHEAD2 'T'
#define CLASSHEAD 'C'
#define DATAHEAD 'D'
#define FILEHEAD 'F'
#define TCPEND1 'O'
#define TCPEND2 'V'

#define HEAD_SIZE 8
		static void set_buf_from_num(char* p, uint32_t n) {
#ifdef _MSC_VER
			memcpy(p, &n, 4);
#else
			p[0] = char(n % 256); n /= 256;	p[1] = char(n % 256); n /= 256;
			p[2] = char(n % 256); n /= 256;	p[3] = char(n % 256);
#endif // _MSC_VER
		}
		static uint32_t get_num_from_buf(char* p) {
#ifdef _MSC_VER
			uint32_t n;	memcpy(&n, p, 4); return n;
#else
			return (p[0] + p[1] * 256 + p[2] * 65536 + p[3] * 256 * 65536);
#endif // _MSC_VER
		}
	

		typedef std::shared_ptr<DataPackage<T>> dataPtr;

		//���ࣺ����------------------------------------------------------------------------
		class TcpConnection :public TcpConnectionBase {
			typedef std::shared_ptr<boost::asio::streambuf> buff_Ptr;
		public:
			TcpConnection(boost::asio::io_service& io_, 
				std::function<void(const TcpEp &)> errcb_,
				tpath* p_RecvPath_,
				std::function<void(dataPtr &)> cb_):
				TcpConnectionBase(io_, errcb_), 
				m_recv_callback(cb_), 
				p_RecvPath(p_RecvPath_){

			}
			virtual ~TcpConnection() {}

			//ͬ�����ͣ�����
			bool write(const std::shared_ptr<std::vector<boost::asio::const_buffer>> & data_) {
				boost::system::error_code err;
				write_mutex.lock();
				sock.write_some(*data_, err);
				write_mutex.unlock();
				if (err) {
					printf_s("write failed : %s\n", err.message().c_str());
					err_CallBack(remote_ep);
					return false;
				}
				return true;
			}
			
			void start() {
				boost::asio::async_read(sock, boost::asio::buffer(header), boost::asio::transfer_exactly(HEAD_SIZE),
					std::bind(&TcpConnection::on_read_head, this, std::placeholders::_1, std::placeholders::_2));
			}

			bool read_get_err(const boost::system::error_code & err) {
				if (err) {
					printf_s("read failed : %s\n", err.message().c_str());
					err_CallBack(remote_ep);
					return true;
				}
				return false;
			}
			//������ɺ��ȿ�����һ���첽�����������ûص�
			void on_read_head(const boost::system::error_code & err, size_t read_bytes) {
				if (read_get_err(err)) return;
				//������ͷ
				if (header[0] == TCPHEAD1 && header[1] == TCPHEAD2 && read_bytes == HEAD_SIZE) {
					uint32_t pack_size = get_num_from_buf(&header[4]);
					if (header[2] == CLASSHEAD) {
						if (RDataPtr) {
							printf_s("read failed : incomplete package\n");
							err_CallBack(remote_ep);
							return;
						}
						RDataPtr = dataPtr(new DataPackage<T>());
						buff_Ptr buff_ = buff_Ptr(new boost::asio::streambuf());
						boost::asio::async_read(sock, *buff_, boost::asio::transfer_exactly(pack_size),
							std::bind(&TcpConnection::on_read_obj, this, buff_, std::placeholders::_1, std::placeholders::_2));
						return;
					}
					if (header[2] == DATAHEAD) {
						if (header[3] == static_cast<char>(uint8_t(255))) {

						}
						else {

						}
						return;
					}
					if (header[2] == FILEHEAD) {
						if (header[3] == static_cast<char>(uint8_t(255))) {

						}
						else {

						}
						return;
					}
					if (header[2] == TCPEND1 && header[3] == TCPEND2) {
						dataPtr re = RDataPtr;
						RDataPtr.reset();
						boost::asio::async_read(sock, boost::asio::buffer(header), boost::asio::transfer_exactly(HEAD_SIZE),
							std::bind(&TcpConnection::on_read_head, this, std::placeholders::_1, std::placeholders::_2));
						//��Ҫ�ȴ����в������
						while (!re.unique());
						m_recv_callback(std::move(re));
						return;
					}
				}

				printf_s("read failed : recv an invalid header\n");
				err_CallBack(remote_ep);
				return;
			}

			void on_read_obj(buff_Ptr& buff_,const boost::system::error_code & err, size_t read_bytes) {
				if (read_get_err(err)) return;
				boost::asio::async_read(sock, boost::asio::buffer(header), boost::asio::transfer_exactly(HEAD_SIZE),
					std::bind(&TcpConnection::on_read_head, this, std::placeholders::_1, std::placeholders::_2));
				boost::archive::binary_iarchive iar(*buff_);
				iar >> RDataPtr->obj;
			}
			void on_read_data_tips(const boost::system::error_code & err, size_t read_bytes) {

			}
			void on_read_data(const boost::system::error_code & err, size_t read_bytes) {

			}
			void on_read_file_tips(const boost::system::error_code & err, size_t read_bytes) {

			}
			void on_read_file(const boost::system::error_code & err, size_t read_bytes) {

			}


		protected:
			std::mutex write_mutex;
			std::function<void(dataPtr &)> m_recv_callback;
			tpath* p_RecvPath;//�����ļ�·��

			//���ջ���
			dataPtr RDataPtr;
			char header[HEAD_SIZE];

		};
		//���ࣺ���ӳ�---------------------------------------------------------------------
		class TcpConnectionPool : public TcpConnectionPoolBase<TcpConnection> {
		public:
			TcpConnectionPool(uint16_t port_, 
				tpath* p_RecvPath_,
				std::function<void(dataPtr &)> cb_) :
				TcpConnectionPoolBase(port_), 
				p_RecvPath(p_RecvPath_),
				m_recv_callback(cb_){

			}
			virtual ~TcpConnectionPool() {}

		protected:
			//��������
			TcpConnectionPtr getNewTcpConnectionPtr() {
				return TcpConnectionPtr(new TcpConnection(service, 
					std::bind(&TcpConnectionPoolBase<TcpConnection>::on_err, this, std::placeholders::_1),
					p_RecvPath,
					m_recv_callback
					));
			}
			std::function<void(dataPtr &)> m_recv_callback;
			tpath* p_RecvPath;//�����ļ�·��
		};
		//�౾��-------------------------------------------------------------------------
	public:
		TcpNetAdapter(uint32_t myid_,
			const TcpEp & hostInfo_,
			std::function<void(std::shared_ptr<DataPackage<T>> &)> recvcb_,
			const tstring& rp = T_TEXT(""),	const tstring& sp = T_TEXT("")) :
			NetAdapterBase(myid_, hostInfo_, recvcb_) ,
			m_RecvPath(tGetAbsolutePath(rp)), m_SendPath(tGetAbsolutePath(sp)),
			m_TcpConnectionPool(hostInfo_.port(),&m_RecvPath, m_receiveCallBack){
			boost::filesystem::create_directories(m_RecvPath);
			boost::filesystem::create_directories(m_SendPath);
			m_TcpConnectionPool.start();
		}
		virtual ~TcpNetAdapter(){}


	protected:
		virtual bool _sendfun(const dataPtr & Tdata_, const std::vector<TcpEp>& dst_) {
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
			//�ڶ�������������
			//�ȷ���tips���ݰ�
			std::map<std::string, shared_buf>& map_datas = Tdata_->map_datas;
			//���������ܷ����ڲ�scope��
			std::string data_tips;
			char d0_head_buff[HEAD_SIZE]{ TCPHEAD1 ,TCPHEAD2,DATAHEAD,static_cast<char>(uint8_t(255)) };
			boost::shared_array<char> d_head_buff = boost::shared_array<char>(new char[map_datas.size() * HEAD_SIZE]);
			if (map_datas.size() > 0) {
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
			
			//�������������ļ�
			//�ȷ����ļ�����Ϣ
			std::map<std::string, std::string>& map_files = Tdata_->map_files;
			std::string file_tips;
			char f0_head_buff[HEAD_SIZE]{ TCPHEAD1 ,TCPHEAD2,FILEHEAD,static_cast<char>(uint8_t(255)) };
			boost::shared_array<char> f_head_buff = boost::shared_array<char>(new char[map_files.size() * HEAD_SIZE]);
			std::vector<boost::shared_array<char>> vec_file_buf;
			if (map_files.size() > 0) {
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
			//���Ĳ������ͽ�����
			char end_head_buff[HEAD_SIZE]{ TCPHEAD1 ,TCPHEAD2,TCPEND1,TCPEND2 };
			buffersPtr->push_back(std::move(boost::asio::const_buffer(end_head_buff, HEAD_SIZE)));

			//����ַ�Ż�
			if (dst_.size() == 1) {
				return _send_one(buffersPtr, dst_[0]);
			}
			//����ʹ�ö��̲߳���Ⱥ��
			std::vector<std::future<bool>> v;
			size_t len = dst_.size();
			for (size_t ii = 0; ii < len; ii++) {
				v.push_back(std::async(std::launch::async, &TcpNetAdapter::_send_one, this, buffersPtr, dst_[ii]));
			}
			bool result = true;
			for (size_t ii = 0; ii < len; ii++) {
				result = result&&(v[ii].get());
			}
			return result;
		}

		virtual bool _send_one(const std::shared_ptr<std::vector<boost::asio::const_buffer>> & Tdata_, const TcpEp & ep) {
			std::shared_ptr<TcpConnection> pc = m_TcpConnectionPool.getTcpConnectionPtr(ep);
			if (!pc) return false;
			return pc->write(Tdata_);
		}

		//��������

		TcpConnectionPool m_TcpConnectionPool;
		tpath m_RecvPath;//���ա������ļ�·��
		tpath m_SendPath;

	};



}