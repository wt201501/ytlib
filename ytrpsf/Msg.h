#pragma once
#include <ytlib/NetTools/TcpNetAdapter.h>
#include <boost/date_time/posix_time/posix_time.hpp>  
#include <ytrpsf/Plugin_Bus_Interface.h>

namespace rpsf {

	//���ļ���ʵ�ָ������ݰ�
	enum MsgType {
		RPSF_DATA = 0,
		RPSF_RPC = 1,
		RPSF_RRPC = 2,
		RPSF_SYS = 3
	};

	//��ײ����ݰ���ʽ���������ݣ� event�����ݣ�RRPC��������Ϣ
	class rpsfMsg {
		T_CLASS_SERIALIZE(&m_srcAddr&m_msgType)
	public:
		rpsfMsg():m_delfiles(false){}

		uint32_t m_srcAddr;//��ϢԴ���id
		uint8_t m_msgType;//��Ϣ���͡���ΪҪ֧�����л���������Ϊuint8

		HandleType m_handleType;//����ʽ
		bool m_delfiles;//������ɺ��Ƿ�ɾ���ļ�
	};
	typedef ytlib::DataPackage<rpsfMsg> rpsfDataPackage;
	typedef std::shared_ptr<rpsfDataPackage> rpsfPackagePtr;

	//���������͵���Ϣ�����Ϊһ��rpsfPackagePtr��ͬʱԭ��Ϣ����������
	static rpsfPackagePtr setBaseMsgToPackage(rpsfPackage& m_) {
		rpsfPackagePtr package = std::make_shared<rpsfDataPackage>();
		package->map_datas.swap(m_.m_mapDatas);
		package->map_files.swap(m_.m_mapFiles);
		package->obj.m_handleType = m_.m_handleType;
		package->obj.m_delfiles = m_.m_bDelFiles;

		return package;
	}
	static rpsfPackagePtr setMsgToPackage(rpsfRpcArgs& m_) {
		rpsfPackagePtr package = setBaseMsgToPackage(m_);
		package->obj.m_msgType = MsgType::RPSF_RPC;
		package->quick_data = ytlib::sharedBuf(m_.m_service);
		return package;
	}
	static rpsfPackagePtr setMsgToPackage(rpsfData& m_) {
		rpsfPackagePtr package = setBaseMsgToPackage(m_);
		package->obj.m_msgType = MsgType::RPSF_DATA;
		package->quick_data = ytlib::sharedBuf(m_.m_dataName);
		return package;
	}
	static rpsfPackagePtr setMsgToPackage(rpsfRpcResult& m_) {
		rpsfPackagePtr package = setBaseMsgToPackage(m_);
		package->obj.m_msgType = MsgType::RPSF_RRPC;
		//BusErr����С��256��.��Ϊ���ǿ�ƽ̨ʱ�Ĵ�С������
		package->quick_data = ytlib::sharedBuf(1 + static_cast<uint32_t>(m_.m_errMsg.size()));
		package->quick_data.buf[0] = static_cast<uint8_t>(m_.m_rpcErr);
		memcpy(package->quick_data.buf.get() + 1, m_.m_errMsg.c_str(), m_.m_errMsg.size());
		return package;
	}

	//��rpsfPackagePtr���Ϊ�������͵���Ϣ��
	static bool getBaseMsgFromPackage(rpsfPackagePtr& package_, rpsfPackage& m_) {
		package_->map_datas.swap(m_.m_mapDatas);
		package_->map_files.swap(m_.m_mapFiles);
		m_.m_handleType = package_->obj.m_handleType;
		return true;
	}
	static bool getMsgFromPackage(rpsfPackagePtr& package_, rpsfRpcArgs& m_) {
		if (!getBaseMsgFromPackage(package_, m_)) return false;
		m_.m_service.assign(package_->quick_data.buf.get(), package_->quick_data.buf_size);
		return true;
	}
	static bool getMsgFromPackage(rpsfPackagePtr& package_, rpsfData& m_) {
		if (!getBaseMsgFromPackage(package_, m_)) return false;
		m_.m_dataName.assign(package_->quick_data.buf.get(), package_->quick_data.buf_size);
		return true;
	}
	static bool getMsgFromPackage(rpsfPackagePtr& package_, rpsfRpcResult& m_) {
		if (!getBaseMsgFromPackage(package_, m_)) return false;
		m_.m_rpcErr = static_cast<BusErr>(static_cast<uint8_t>(package_->quick_data.buf[0]));
		m_.m_errMsg.assign(package_->quick_data.buf.get() + 1, package_->quick_data.buf_size - 1);
		return true;
	}
}


