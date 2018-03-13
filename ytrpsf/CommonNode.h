#pragma once
#include <ytrpsf/RpsfCfgFile.h>
#include <ytrpsf/Bus.h>
#include <ytlib/SupportTools/SysInfoTools.h>
#include <boost/date_time/posix_time/posix_time.hpp>  

namespace rpsf {
	//��ͨ�ڵ�
	class CommonNode : public Bus{
	public:
		CommonNode() :Bus() {}
		virtual ~CommonNode() {
			Stop();
		}

		virtual bool Init(const std::string& cfgpath) {

			RpsfCfgFile cfgfile;
			try {
				cfgfile.OpenFile(T_STRING_TO_TSTRING(cfgpath));
			}
			catch (const ytlib::Exception& e) {
				std::cout << e.what() << std::endl;
				return false;
			}
			RpsfNode& thisnode = *(cfgfile.m_fileobj.get());
			if (thisnode.NodeType != RpsfNodeType::NODETYPE_COMMON) {
				tcout << T_TEXT("error : node type mismatch! please check the cfg file and exe type.") << std::endl;
				return false;
			}

			if (!(Bus::Init(thisnode))) return false;

			//�������Ľڵ���Ϣ
			m_netAdapter->SetHost(thisnode.CenterNodeId, thisnode.CenterNodeIp, thisnode.CenterNodePort);
			
			//����ϵͳ�¼�
			std::set<SysMsgType> sysevents{
				SysMsgType::SYS_NEW_NODE_ONLINE,
				SysMsgType::SYS_ALL_INFO,
				SysMsgType::SYS_SUB_DATAS,
				SysMsgType::SYS_SUB_SERVICES,
				SysMsgType::SYS_SUB_SYSEVENT,
				SysMsgType::SYS_HEART_BEAT };
			Bus::SubscribeSysEvent(sysevents);
			//ע��
			
			newNodeRegMsg msg{ m_NodeId ,sysevents ,thisnode.NodeName,thisnode.NodeIp,thisnode.NodePort };
			rpsfPackagePtr p = setSysMsgToPackage(msg, SysMsgType::SYS_NEW_NODE_REG);
			p->obj.m_handleType = HandleType::RPSF_SYNC;
			p->obj.m_pushList.insert(thisnode.CenterNodeId);
			regFlag.reset();
			rpsfMsgHandler(p);//ͬ������

			//�ȴ����յ��ظ���Ϣ�����һ��ʱ��û��ע��ɹ�����ʾע��ʧ��
			if (!regFlag.wait_for(5000)) {
				//����5s��δ�յ��ظ���Ϣ
				tcout << T_TEXT("Registration failed: timeout!") << std::endl;
				return false;
			}
			//ע��ɹ�������ϵͳ����¼�
			SubscribeSysEvent(std::set<SysMsgType>{SysMsgType::SYS_HEART_BEAT});

			//���ظ������
			rpsfLoadPlugins(thisnode.PluginSet);

			m_bRunning = true;
			return true;

		}


	private:
		virtual void SubscribeSysEvent(const std::set<SysMsgType>& sysEvents_) {
			Bus::SubscribeSysEvent(sysEvents_);
			subscribeSysEventMsg msg{ m_NodeId ,sysEvents_ ,true };
			rpsfPackagePtr p = setSysMsgToPackage(msg, SysMsgType::SYS_SUB_SYSEVENT);
			p->obj.m_handleType = HandleType::RPSF_SYNC;
			rpsfMsgHandler(p);//ͬ������
		}
		virtual void UnsubscribeSysEvent(const std::set<SysMsgType>& sysEvents_) {
			Bus::UnsubscribeSysEvent(sysEvents_);
			subscribeSysEventMsg msg{ m_NodeId ,sysEvents_ ,false };
			rpsfPackagePtr p = setSysMsgToPackage(msg, SysMsgType::SYS_SUB_SYSEVENT);
			p->obj.m_handleType = HandleType::RPSF_SYNC;
			rpsfMsgHandler(p);//ͬ������
		}
		virtual void rpsfSysHandler(rpsfSysMsg& m_) {
			//���±�ʱҪע�⣬ֻ�ܸ��������ڵ����Ϣ���Լ����ص���Ϣ������������Ĳ�һ����Ҫ���Ͼ���
			switch (m_.m_sysMsgType) {
			case SysMsgType::SYS_HEART_BEAT: {
				//�յ������¼�
				heartBeatMsg msg;
				getSysMsgFromPackage(m_, msg);
				heartBeatResponseMsg msg2{ m_NodeId,msg.heartBeatIndex ,msg.heartBeatTime,ytlib::GetCpuUsage(),ytlib::GetMemUsage() };

				rpsfPackagePtr p = setSysMsgToPackage(msg2, SysMsgType::SYS_HEART_BEAT_RESPONSE);
				p->obj.m_handleType = HandleType::RPSF_SYNC;
				rpsfMsgHandler(p);//ͬ������

				break;
			}
			case SysMsgType::SYS_ALL_INFO: {
				//�յ�����ȫ����Ϣ�¼�
				allInfoMsg msg;

				getSysMsgFromPackage(m_, msg);

				//����ϵͳ�¼���ڵ�id�ı�
				std::set<SysMsgType> delset, addset;
				syncSysMsgType2NodeId(msg.mapSysMsgType2NodeId, delset, addset);
				if (!delset.empty()) UnsubscribeSysEvent(delset);
				if (!addset.empty()) SubscribeSysEvent(addset);




				break;
			}
			case SysMsgType::SYS_NEW_NODE_REG_RESPONSE: {
				//�յ��½ڵ�ע����Ӧ�¼�
				newNodeRegResponseMsg msg;
				getSysMsgFromPackage(m_, msg);

				//����ϵͳ�¼���ڵ�id�ı�
				std::set<SysMsgType> delset, addset;
				syncSysMsgType2NodeId(msg.mapSysMsgType2NodeId, delset, addset);
				if (!delset.empty()) UnsubscribeSysEvent(delset);
				if (!addset.empty()) SubscribeSysEvent(addset);




				regFlag.notify();
				break;
			}
			case SysMsgType::SYS_COUNT: {

				break;
			}
			default:
				Bus::rpsfSysHandler(m_);
				break;
			}
		}


		ytlib::LightSignal regFlag;

	};

}


