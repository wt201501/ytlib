#pragma once
#include <ytrpsf/Plugin_Bus_Interface.h>
#include <ytrpsf/Msg.h>
#include <ytlib/SupportTools/Serialize.h>
#include <ytlib/SupportTools/Serialize.h>

namespace rpsf {
	//�������ϵͳ��Ϣ���������͵�ϵͳ��Ϣ���������ݵ������л���buff�У��յ�ʱ�ٷ����л�����
	//ϵͳ�¼�ָ����ͨ�ڵ������Ľڵ���еĽ����¼�����ͨ�ڵ�֮�䲻���ܷ���ϵͳ�¼�
	//���綩��/ȡ���������ݵ��¼�����ͨ�ڵ㽫����Ϣ���͸����Ľڵ㣬���Ľڵ��յ����ٹ㲥�����нڵ�
	//ϵͳ��Ϣ���͡����256��
	enum SysMsgType {
		SYS_NEW_NODE_REG,//�½ڵ������Ľڵ�ע���¼���com�������ͣ�
		SYS_NEW_NODE_REG_RESPONSE,//���Ľڵ�ظ��½ڵ�ע���¼���cen�������ͣ�
		SYS_NEW_NODE_ONLINE,//�½ڵ������¼��������Ľڵ㷢���������½ڵ����Ϣ�����ĵ�ϵͳ�¼�����cen��
		SYS_ALL_INFO,//�����������統ǰ״̬�������Ľڵ㷢����һ����������ߵĽڵ����ȫ����Ϣ��cen��
		SYS_SUB_DATAS,//����/ȡ����������
		SYS_SUB_SERVICES,//ע��/ȡ��ע��RPC
		SYS_SUB_SYSEVENT,//����/ȡ������ϵͳ�¼�
		SYS_HEART_BEAT,//�����¼���cen��
		SYS_HEART_BEAT_RESPONSE,//������Ӧ�¼���com��
		SYS_COUNT	//������ͬʱҲ����Чֵ
	};

	class rpsfSysMsg : public rpsfPackage {
	public:
		rpsfSysMsg(){}
		rpsfSysMsg(SysMsgType sysMsgType_):m_sysMsgType(sysMsgType_){}
		SysMsgType m_sysMsgType;

	};

	static rpsfPackagePtr setMsgToPackage(rpsfSysMsg& m_) {
		rpsfPackagePtr package = setBaseMsgToPackage(m_);
		package->obj.m_msgType = MsgType::RPSF_SYS;
		package->obj.m_handleType = HandleType::RPSF_SYNC;//Ĭ��Ϊͬ������
		package->quick_data = ytlib::sharedBuf(1);
		package->quick_data.buf[0] = static_cast<uint8_t>(m_.m_sysMsgType);
		return std::move(package);
	}
	static void getMsgFromPackage(rpsfPackagePtr& package_, rpsfSysMsg& m_) {
		getBaseMsgFromPackage(package_, m_);
		m_.m_sysMsgType = static_cast<SysMsgType>(static_cast<uint8_t>(package_->quick_data.buf[0]));
	}
	static SysMsgType getSysMsgTypeFromPackage(const rpsfPackagePtr& package_) {
		return static_cast<SysMsgType>(static_cast<uint8_t>(package_->quick_data.buf[0]));
	}

#define SYSTAG "stg"
	template<class T>
	rpsfPackagePtr setSysMsgToPackage(T& m_, SysMsgType type_) {
		rpsfSysMsg sysMsg(type_);
		char* p; size_t len;
		SERIALIZE_INIT;
		SERIALIZE(m_, p, len);
		sysMsg.addData(SYSTAG, p, len);
		return setMsgToPackage(sysMsg);
	}
	template<class T>
	void getSysMsgFromPackage(rpsfPackagePtr& package_, T& m_) {
		std::map<std::string, ytlib::sharedBuf >::iterator itr = package_->map_datas.find(SYSTAG);
		DESERIALIZE_INIT;
		DESERIALIZE(m_, itr->second.buf.get(), itr->second.buf_size);
	}
	template<class T>
	void getSysMsgFromPackage(rpsfSysMsg& package_, T& m_) {
		std::map<std::string, ytlib::sharedBuf >::iterator itr = package_.m_mapDatas.find(SYSTAG);
		DESERIALIZE_INIT;
		DESERIALIZE(m_, itr->second.buf.get(), itr->second.buf_size);
	}

	//�½ڵ������Ľڵ�ע���¼����ݰ�
	class newNodeRegMsg {
		T_CLASS_SERIALIZE(&NodeId&SysMsg&NodeName)
	public:
		uint32_t NodeId;//�ڵ�id
		std::set<SysMsgType> SysMsg;//�ڵ㶩�ĵ�ϵͳ�¼�
		std::string NodeName;//�ڵ����ƣ�����Ϊ�ڵ㲹����Ϣ��


	};
	//���Ľڵ�㲥�½ڵ������¼����ݰ�
	typedef newNodeRegMsg newNodeOnlineMsg;
	//ȫ����Ϣ���ݰ�
	class allInfoMsg {
		T_CLASS_SERIALIZE(&mapSysMsgType2NodeId&mapDataNmae2NodeId&mapService2NodeId)
	public:
		std::map<SysMsgType, std::set<uint32_t> > mapSysMsgType2NodeId;
		std::map<std::string, std::set<uint32_t> > mapDataNmae2NodeId;
		std::map<std::string, std::set<uint32_t> > mapService2NodeId;
	};
	typedef allInfoMsg newNodeRegResponseMsg;
	//�����¶��������¼����ݰ�
	class subscribeDatasInfoMsg {
		T_CLASS_SERIALIZE(&NodeId&DataNames&Operation)
	public:
		uint32_t NodeId;//�ڵ�id
		std::set<std::string> DataNames;//���ĵ�����
		bool Operation;//������true���ģ�falseȡ������
	};
	//������ע��RPC�¼����ݰ�
	class servicesInfoMsg {
		T_CLASS_SERIALIZE(&NodeId&Services&Operation)
	public:
		uint32_t NodeId;//�ڵ�id
		std::set<std::string> Services;//�ṩ�ķ���
		bool Operation;//������true�ṩ����falseȡ���ṩ����
	};
	//��������ϵͳ�¼����ݰ�
	class subscribeSysEventMsg {
		T_CLASS_SERIALIZE(&NodeId&SysMsg&Operation)
	public:
		uint32_t NodeId;//�ڵ�id
		std::set<SysMsgType> SysMsg;//�ڵ㶩�ĵ�ϵͳ�¼�
		bool Operation;//������true���ģ�falseȡ������
	};
	//��������¼����ݰ�
	class heartBeatMsg {
		T_CLASS_SERIALIZE(&heartBeatIndex&heartBeatTime)
	public:
		uint32_t heartBeatIndex;
		uint64_t heartBeatTime;
	};
	//��������¼���Ӧ���ݰ�
	class heartBeatResponseMsg {
		T_CLASS_SERIALIZE(&nodeId&heartBeatIndex&heartBeatTime&cpuUsage&memUsage)
	public:
		uint32_t nodeId;
		uint32_t heartBeatIndex;
		uint64_t heartBeatTime;

		double cpuUsage;
		double memUsage;

	};

}


