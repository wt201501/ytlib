#pragma once
#include <ytlib/NetTools/NetAdapterBase.h>
#include <boost/asio.hpp>

namespace ytlib {

	/*
	ʹ��boost.asio��һ����������������
	*/
	
	struct DataPackage {
		std::shared_ptr<std::string> dataPtr;//����
		std::vector<tstring> files;	//�ļ�
	};

	typedef boost::asio::ip::tcp::endpoint tcped;//28���ֽ�

	class TCPNetAdapter : public NetAdapterBase<tcped, DataPackage> {
	public:
		TCPNetAdapter(uint32_t myid_, const tcped & hostInfo_,
			std::function<void(DataPackage &)> recvcb_) :NetAdapterBase(myid_, hostInfo_, recvcb_) {

		}
		virtual ~TCPNetAdapter(){}
	protected:
		void _sendfun(const _TransData_dst& Tdata_) {
			//ֱ���ö�ȡ������ȡep


		}
		//��������




	};



}