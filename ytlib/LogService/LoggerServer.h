#pragma once
#include <ytlib/NetTools/TcpNetUtil.h>
#include <sqlite/sqlite3.h>
#include <boost/thread.hpp>

//todo�� ʹ��boost.log�⡢�����������������ݿ������һ����־������

namespace ytlib {

	class LoggerServer {
	private:


		//�����ࡣtagʼ��Ϊ LG
		class LogConnection :boost::noncopyable {
		public:
			LogConnection(boost::asio::io_service& io_) :
				sock(io_){

			}


			boost::asio::ip::tcp::socket sock;
		private:

		};
	public:



	};
	
}