#pragma once
#include <ytrpsf/RpsfCfgFile.h>
#include <ytrpsf/Bus.h>

namespace rpsf {
	//��ͨ�ڵ�
	class CommonNode : public Bus{

	public:
		CommonNode() :Bus() {}
		virtual ~CommonNode() {}

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
			Bus::SubscribeSysEvent(std::set<SysMsgType>());
			//ע��

			//�ȴ����յ��ظ���Ϣ


			//ע��ɹ�������ϵͳ����¼�

			//���ظ������

			



			m_bInit = true;
			return true;

		}
		virtual void SubscribeSysEvent(const std::set<SysMsgType>& sysEvents_) {
			Bus::SubscribeSysEvent(sysEvents_);
			subscribeSysEventMsg msg{ m_NodeId ,sysEvents_ ,true };
			rpsfPackagePtr p = setSysMsgToPackage(msg, SysMsgType::SYS_SUB_SYSEVENT);
			rpsfMsgHandlerLocal(p);//ͬ������
		}
		virtual void UnsubscribeSysEvent(const std::set<SysMsgType>& sysEvents_) {
			Bus::UnsubscribeSysEvent(sysEvents_);
			subscribeSysEventMsg msg{ m_NodeId ,sysEvents_ ,false };
			rpsfPackagePtr p = setSysMsgToPackage(msg, SysMsgType::SYS_SUB_SYSEVENT);
			rpsfMsgHandlerLocal(p);//ͬ������
		}
		virtual void rpsfSysHandler(rpsfSysMsg& m_) {
			switch (m_.m_sysMsgType) {
			case SysMsgType::SYS_TEST1: {


				break;
			}
			case SysMsgType::SYS_TEST2: {


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

	};

}


