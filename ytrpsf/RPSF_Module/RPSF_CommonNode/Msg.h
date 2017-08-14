#pragma once
#include <ytlib/NetTools/TcpNetAdapter.h>
#include <boost/date_time/posix_time/posix_time.hpp>  

namespace rpsf {

	//���ļ���ʵ�ָ������ݰ�
	enum MsgType {
		RPSF_DATA = 0,
		RPSF_EVENT = 1 * 16,
		RPSF_RPC = 2 * 16,
		RPSF_RRPC = 3 * 16,
		RPSF_SYS = 4 * 16
	};

	//��ײ����ݰ���ʽ���������ݣ� event�����ݣ�RRPC��������Ϣ
	class rpsfMsg {
		T_CLASS_SERIALIZE(&m_srcAddr&m_msgType&s1&s2&i1&i2)
	public:
		rpsfMsg():m_delfiles(false){}

		uint32_t m_srcAddr;//��ϢԴ���id
		uint8_t m_msgType;//��Ϣ���͡���ΪҪ֧�����л���������Ϊint
		//һЩ���ٷ������ݿ��Է�����
		std::string s1;//data\event���������ƣ�RPC��service����
		std::string s2;//data\event�������ߣ�RPC��fun����
		uint32_t i1;//event��ʱ���32λ��RPC\RRPC��rpcID 
		uint32_t i2;//event��ʱ���32λ��RPC��timeout��RRPC������

		bool m_delfiles;//������ɺ��Ƿ�ɾ���ļ�
	};
	typedef ytlib::DataPackage<rpsfMsg> rpsfDataPackage;
	typedef std::shared_ptr<rpsfDataPackage> rpsfPackagePtr;

}


