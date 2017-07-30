#pragma once
#include <ytlib/NetTools/TcpNetUtil.h>
#include <ytlib/Common/Util.h>
#include <shared_mutex>
#include <boost/thread.hpp>

namespace ytlib
{
	//������sock����
	class ConnectionBase :boost::noncopyable {
	public:
		enum {
			TCPHEAD1 = 'Y',
			TCPHEAD2 = 'T',
			TCPEND1 = 'O',
			TCPEND2 = 'V'
		};
		static const uint8_t HEAD_SIZE = 8;
		ConnectionBase(boost::asio::io_service& io_, std::function<void(const TcpEp &)> errcb_) :
			sock(io_), err_CallBack(errcb_), stopflag(false) {

		}
		virtual ~ConnectionBase() {
			stopflag = true;
		}
		virtual void start() { do_read_head(); }

		TcpSocket sock;
		TcpEp remote_ep;

	protected:
		virtual void do_read_head() {
			boost::asio::async_read(sock, boost::asio::buffer(header, HEAD_SIZE), boost::asio::transfer_exactly(HEAD_SIZE),
				std::bind(&ConnectionBase::on_read_head, this, std::placeholders::_1, std::placeholders::_2));
		}
		bool read_get_err(const boost::system::error_code & err) {
			if (stopflag) return true;
			if (err) {
				stopflag = true;
				YT_DEBUG_PRINTF("read failed : %s\n", err.message().c_str());
				err_CallBack(remote_ep);
				return true;
			}
			return false;
		}
		virtual void on_read_head(const boost::system::error_code & err, size_t read_bytes) {
			if (read_get_err(err)) return;
			if (header[0] == TCPHEAD1 && header[1] == TCPHEAD2 && read_bytes == HEAD_SIZE) {
				uint32_t pack_size = get_num_from_buf(&header[4]);
				//do something
				return;
			}
			stopflag = true;
			YT_DEBUG_PRINTF("read failed : recv an invalid header : %c %c %c %d %d\n",
				header[0], header[1], header[2], header[3], get_num_from_buf(&header[4]));
			err_CallBack(remote_ep);
			return;
		}
		std::atomic_bool stopflag;
		std::function<void(const TcpEp &)> err_CallBack;//��������ʱ�Ļص���һ����/д�����͹ر����Ӳ����ûص���֪�ϲ�
		char header[HEAD_SIZE];//���ջ���
	};


	//tcp���ӳء���Ҫ�̳�����getNewTcpConnectionPtr����ʹ�á�ֻ������������������
	template<class T_Connection>
	class TcpConnectionPool {
	protected:
		typedef std::shared_ptr<T_Connection> TcpConnectionPtr;

		std::atomic_bool stopflag;//ֹͣ��־
		boost::thread_group m_RunThreads;

		std::map<TcpEp, TcpConnectionPtr> m_mapTcpConnection;//Ŀ��ep-TcpConnection��map
		std::shared_mutex m_TcpConnectionMutex;

		std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptorPtr;//������
		boost::asio::io_service service;

		const uint32_t m_threadSize;//ʹ�õ��첽�߳�����
		const uint16_t myport;//�����˿ڣ����������������е����Ӷ��󶨵�����˿���

		virtual TcpConnectionPtr getNewTcpConnectionPtr() {
			return TcpConnectionPtr();
		}
		virtual void on_accept(TcpConnectionPtr& p, const boost::system::error_code & err) {
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
			if (!pConnection) {
				stop();
				return;
			}
			acceptorPtr->async_accept(pConnection->sock, std::bind(&TcpConnectionPool::on_accept, this, pConnection, std::placeholders::_1));

		}
		virtual void on_err(const TcpEp& ep) {
			YT_DEBUG_PRINTF("connection to %s:%d get an err and is closed\n", ep.address().to_string().c_str(), ep.port());
			std::unique_lock<std::shared_mutex> lck(m_TcpConnectionMutex);
			std::map<TcpEp, TcpConnectionPtr>::iterator itr = m_mapTcpConnection.find(ep);
			if (itr != m_mapTcpConnection.end()) {
				m_mapTcpConnection.erase(itr);
			}
		}

	public:
		TcpConnectionPool(uint16_t port_,
			uint32_t threadSize_ = 10) :
			myport(port_),
			m_threadSize(threadSize_),
			stopflag(true) {

		}
		virtual ~TcpConnectionPool() {
			stop();
		}
		virtual bool start() {
			if (!checkPort(myport)) return false;
			TcpConnectionPtr pConnection = getNewTcpConnectionPtr();
			if (!pConnection) return false;
			service.reset();
			stopflag = false;
			acceptorPtr = std::shared_ptr<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor(service, TcpEp(boost::asio::ip::tcp::v4(), myport), true));
			acceptorPtr->async_accept(pConnection->sock, std::bind(&TcpConnectionPool::on_accept, this, pConnection, std::placeholders::_1));
			for (uint32_t i = 0; i < m_threadSize; ++i) {
				m_RunThreads.create_thread(boost::bind(&boost::asio::io_service::run, &service));
			}
			return true;
		}
		virtual void stop() {
			stopflag = true;
			service.stop();
			acceptorPtr.reset();
			std::unique_lock<std::shared_mutex> lck(m_TcpConnectionMutex);
			m_mapTcpConnection.swap(std::map<TcpEp, TcpConnectionPtr>());
			m_RunThreads.join_all();
		}
	};

	
}