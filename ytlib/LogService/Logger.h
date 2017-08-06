#pragma once
#include <ytlib/NetTools/TcpNetUtil.h>
#include <ytlib/Common/Util.h>
#include <ytlib/LogService/LoggerServer.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>



namespace ytlib {
	//ͬ����ˣ�����Ҫ����
	class NetBackend : public boost::log::sinks::basic_formatted_sink_backend<
		char,boost::log::sinks::synchronized_feeding> {
	public:
		//Ŀǰֻ֧��int��id�����Ҫ�ĳ�string��idҲ�ܼ�
		explicit NetBackend(uint32_t myid_,const TcpEp& logserver_ep_ ):
			sock(service), LogServerEp(logserver_ep_), ConnectFlag(false){
			header[0] = LogConnection::TCPHEAD1;
			header[1] = LogConnection::TCPHEAD2;
			header[2] = LogConnection::LOGHEAD1;
			header[3] = LogConnection::LOGHEAD2;
			logBuff.push_back(std::move(boost::asio::const_buffer(header, HEAD_SIZE)));
			//���ñ���id������Ժ�Ҫ��ӱ�����ϢҲ�ڴ˴������չ
			HostInfoSize = 4;
			HostInfoBuff = boost::shared_array<char>(new char[HostInfoSize]);
			set_buf_from_num(HostInfoBuff.get(), myid_);
			logBuff.push_back(std::move(boost::asio::const_buffer(HostInfoBuff.get(), HostInfoSize)));
			connect();
		}
		void consume(boost::log::record_view const& rec, std::string const& command_line) {
			//��������Ӳŷ��ͣ����򲻷���
			if (connect()) {
				set_buf_from_num(&header[4], static_cast<uint32_t>(command_line.size() + HostInfoSize));
				logBuff.push_back(std::move(boost::asio::buffer(command_line)));
				boost::system::error_code err;
				//����ʧ����ConnectFlag��Ϊfalse
				sock.write_some(logBuff, err);
				logBuff.pop_back();
				if (err) {
					ConnectFlag = false;
					YT_DEBUG_PRINTF("send to log server failed : %s\n", err.message().c_str());
					return;
				}
			}
		}
	private:
		bool connect() {
			if (ConnectFlag) return true;
			sock.open(boost::asio::ip::tcp::v4());
			boost::system::error_code err;
			sock.connect(LogServerEp, err);
			if (err) {
				YT_DEBUG_PRINTF("connect to log server failed : %s\n", err.message().c_str());
				return false;
			}
			ConnectFlag = true;
			return true;
		}

		boost::asio::io_service service;//ȫͬ�����������Բ���Ҫrun
		TcpSocket sock;
		TcpEp LogServerEp;
		std::atomic_bool ConnectFlag;
		std::vector<boost::asio::const_buffer> logBuff;
		boost::shared_array<char> HostInfoBuff;
		uint32_t HostInfoSize;
		static const uint8_t HEAD_SIZE = 8;
		char header[HEAD_SIZE];//��ͷ����

	};

	//��־�������ġ��ṩȫ�ֵ�����Ĭ��ʹ�ÿ���̨��־����Ҫ�ֶ�����������־
	class LogControlCenter {
	public:
		LogControlCenter() {

		}
		~LogControlCenter() {

		}
		void EnableConsoleLog() {

		}
		void DisableNetLog() {

		}
		void EnableNetLog() {

		}
		void DisableConsoleLog() {

		}
		

	private:

	};

	//һЩ�궨��
#define YT_LOG_TRACE 
#define YT_LOG_DEBUG
#define YT_LOG_INFO
#define YT_LOG_WARNING
#define YT_LOG_ERROR
#define YT_LOG_FATAL

}