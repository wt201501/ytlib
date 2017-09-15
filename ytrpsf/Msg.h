#pragma once
#include <ytlib/NetTools/TcpNetAdapter.h>
#include <boost/date_time/posix_time/posix_time.hpp>  
#include <ytrpsf/Plugin_Bus_Interface.h>

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
		T_CLASS_SERIALIZE(&m_srcAddr&m_msgType)
	public:
		rpsfMsg():m_delfiles(false){}

		uint32_t m_srcAddr;//��ϢԴ���id
		uint8_t m_msgType;//��Ϣ���͡���ΪҪ֧�����л���������Ϊint

		bool m_delfiles;//������ɺ��Ƿ�ɾ���ļ�
	};
	typedef ytlib::DataPackage<rpsfMsg> rpsfDataPackage;
	typedef std::shared_ptr<rpsfDataPackage> rpsfPackagePtr;

	//���������͵���Ϣ�����Ϊһ��rpsfPackagePtr
	static rpsfPackagePtr setMsgToPackage(const rpsfRpcArgs& m_) {
		rpsfPackagePtr package = rpsfPackagePtr(new rpsfDataPackage());

		return package;
	}
	static rpsfPackagePtr setMsgToPackage(const rpsfData& m_) {
		rpsfPackagePtr package = rpsfPackagePtr(new rpsfDataPackage());

		return package;
	}
	static rpsfPackagePtr setMsgToPackage(const rpsfRpcResult& m_) {
		rpsfPackagePtr package = rpsfPackagePtr(new rpsfDataPackage());

		return package;
	}

	//��rpsfPackagePtr���Ϊ�������͵���Ϣ��
	static bool getMsgFromPackage(rpsfPackagePtr& package_, rpsfRpcArgs& m_) {

		return true;
	}
	static bool getMsgFromPackage(rpsfPackagePtr& package_, rpsfData& m_) {

		return true;
	}
	static bool getMsgFromPackage(rpsfPackagePtr& package_, rpsfRpcResult& m_) {

		return true;
	}
}


