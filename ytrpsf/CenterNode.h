#pragma once
#include <ytrpsf/Bus.h>

namespace rpsf {

	//���Ľڵ㡣�ṩһϵ�й����ؽӿ�
	class CenterNode : public Bus {
	public:
		CenterNode() :Bus() {}
		virtual ~CenterNode() {
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
			if (thisnode.NodeType != RpsfNodeType::NODETYPE_CENTER) {
				tcout << T_TEXT("error : node type mismatch! please check the cfg file and exe type.") << std::endl;
				return false;
			}


			if (!(Bus::Init(thisnode))) return false;

			//����ϵͳ�¼�
			Bus::SubscribeSysEvent(std::set<SysMsgType>{
				SysMsgType::SYS_SUB_DATAS,
				SysMsgType::SYS_SUB_SERVICES,
				SysMsgType::SYS_SUB_SYSEVENT,
				SysMsgType::SYS_HEART_BEAT_RESPONSE
			});

			//������������
			
			m_pheartBeatThread = std::make_unique<std::thread>(std::bind(&CenterNode::heartBeatThreadFun, this));

			//���ظ������
			rpsfLoadPlugins(thisnode.PluginSet);

			m_bRunning = true;
			return true;
		}

		virtual bool Stop() {
			if (!m_bRunning) return true;//�Ѿ�ֹͣ��
			if (!Bus::Stop()) return false;
			m_pheartBeatThread->join();
			return true;
		}
	private:
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

		//�������̡����м��ҵ�񣬲����ϸ�ĵ�ʱ���д���
		void heartBeatThreadFun() {
			uint32_t heartBeatIndex = 0;
			while (m_bRunning) {
				std::this_thread::sleep_for(std::chrono::milliseconds(3000));//3sһ��
				//��������ÿ��һ��ʱ��㲥һ�������¼�����ͨ�ڵ������յ������¼��������Ľڵ�ظ�������Ϣ�����Ľڵ���
				//��������ÿ��һ��ʱ����һ����ͨ�ڵ����Ϣ
				heartBeatMsg msg;
				msg.heartBeatIndex = heartBeatIndex;


				rpsfPackagePtr p = setSysMsgToPackage(msg, SysMsgType::SYS_SUB_SYSEVENT);
				p->obj.m_handleType = HandleType::RPSF_SYNC;
				rpsfMsgHandler(p);//ͬ������

				//һ���������������һ��ȫ����Ϣ����
				if (heartBeatIndex % 10 == 0) {
					//ȫ������һ�����������ٽ���ȫ�����£��������µı�Ķ�����������Ϊ��



				}

				++heartBeatIndex;
			}
		}


		std::unique_ptr<std::thread> m_pheartBeatThread;
	};

}


