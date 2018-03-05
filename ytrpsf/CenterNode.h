#pragma once
#include <ytrpsf/Bus.h>
#include <boost/date_time/posix_time/posix_time.hpp>  
#include <chrono>

namespace rpsf {

	enum NodeState {
		NODE_STATE_NORMAL,
		NODE_STATE_DELAY,
		NODE_STATE_NO_RESPONSE,
		NODE_STATE_LOST,
		NODE_STATE_NEW_NODE
	};
	class nodeInfo {
	public:
		nodeInfo(uint32_t nodeId_,const std::string& nodeName_, std::chrono::system_clock::time_point regTime_, uint32_t curHeartBeatIndex_)
			:nodeId(nodeId_), nodeName(nodeName_), regTime(regTime_),
			curHeartBeatIndex(curHeartBeatIndex_), curPing(0), state(NodeState::NODE_STATE_NEW_NODE){

		}
		std::shared_mutex nodeInfoMutex;

		const uint32_t nodeId;
		const std::string nodeName;
		const std::chrono::system_clock::time_point regTime;//ע��ʱ��

		uint32_t curHeartBeatIndex;//�������
		uint64_t curPing;//Ping��us�����Ľڵ㷢��������Ϣ�����Ľڵ��յ�������Ϣ��ʱ��

		NodeState state;//״̬
		double cpuUsage;
		double memUsage;

		std::set<std::string> plugins;//���صĲ��

		void updataState(uint32_t curHeartBeatIndex_) {
			//���ping>1���������ж�Ϊdelay


			//�����3���������ж�Ϊ����Ӧ

			//�����10���������ж�Ϊ���ߣ���������
		}
	};
	typedef std::shared_ptr<nodeInfo> nodeInfoPtr;

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

			//ע���Լ�
			std::unique_lock<std::shared_mutex> lck(m_mapNodeInfoMutex);
			m_mapNodeInfo.insert(std::pair<uint32_t, nodeInfoPtr>(m_NodeId, std::make_shared<nodeInfo>(m_NodeId, thisnode.NodeName, std::chrono::system_clock::now(), heartBeatIndex)));
			lck.unlock();


			//���ظ������
			rpsfLoadPlugins(thisnode.PluginSet);

			m_bRunning = true;

			//������������
			m_pheartBeatThread = std::make_unique<std::thread>(std::bind(&CenterNode::heartBeatThreadFun, this));
			//m_pheartBeatThread->detach();

			

			
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
			case SysMsgType::SYS_NEW_NODE_REG: {
				newNodeRegMsg msg;
				getSysMsgFromPackage(m_, msg);
				syncSysMsgType2NodeId(msg.SysMsg, msg.NodeId);

				newNodeRegResponseMsg msg1;
				std::shared_lock<std::shared_mutex> lck(m_mapSysMsgType2NodeIdMutex);
				msg1.mapSysMsgType2NodeId = m_mapSysMsgType2NodeId;
				lck.unlock();
				std::shared_lock<std::shared_mutex> lck1(m_mapDataNmae2NodeIdMutex);
				msg1.mapDataNmae2NodeId = m_mapDataNmae2NodeId;
				lck1.unlock();
				std::shared_lock<std::shared_mutex> lck2(m_mapService2NodeIdMutex);
				msg1.mapService2NodeId = m_mapService2NodeId;
				lck2.unlock();

				rpsfPackagePtr p1 = setSysMsgToPackage(msg1, SysMsgType::SYS_NEW_NODE_REG_RESPONSE);
				p1->obj.m_handleType = HandleType::RPSF_SYNC;
				p1->obj.m_pushList.insert(msg.NodeId);
				rpsfMsgHandler(p1);//ͬ������

				std::unique_lock<std::shared_mutex> lck3(m_mapNodeInfoMutex);
				m_mapNodeInfo.insert(std::pair<uint32_t, nodeInfoPtr>(msg.NodeId,std::make_shared<nodeInfo>(msg.NodeId, msg.NodeName, std::chrono::system_clock::now(), heartBeatIndex)));

				break;
			}
			case SysMsgType::SYS_HEART_BEAT_RESPONSE: {
				heartBeatResponseMsg msg;
				getSysMsgFromPackage(m_, msg);

				std::shared_lock<std::shared_mutex> lck(m_mapNodeInfoMutex);
				std::map<uint32_t, nodeInfoPtr>::iterator itr = m_mapNodeInfo.find(msg.nodeId);
				if (itr != m_mapNodeInfo.end()) {
					std::unique_lock<std::shared_mutex> lck1(itr->second->nodeInfoMutex);
					//��ֹ����Ϣ�ھ���Ϣ֮ǰ��
					if (itr->second->curHeartBeatIndex < msg.heartBeatIndex) {
						itr->second->curHeartBeatIndex = msg.heartBeatIndex;

						itr->second->cpuUsage = msg.cpuUsage;
						itr->second->memUsage = msg.memUsage;

						boost::posix_time::ptime tnow = boost::posix_time::microsec_clock::universal_time();
						uint64_t itnow;
						memcpy(&(itnow), &tnow, 8);
						itr->second->curPing = itnow - msg.heartBeatTime;

						//����һ���жϣ��޸���״̬
						itr->second->updataState(heartBeatIndex);
					}
				}
				else {
					//û���ҵ����node
					lck.unlock();

				}
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

			while (m_bRunning) {
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));//2sһ��
				//�ȼ��һ�¸����ڵ��״̬
				std::shared_lock<std::shared_mutex> lck3(m_mapNodeInfoMutex);
				for (std::map<uint32_t, nodeInfoPtr>::iterator itr = m_mapNodeInfo.begin(); itr != m_mapNodeInfo.end(); ++itr) {
					std::unique_lock<std::shared_mutex> lck1(itr->second->nodeInfoMutex);
					itr->second->updataState(heartBeatIndex);
					//����ʱ��ӡһ�¸����ڵ�״̬
					printf("%s-%d:%d\n", itr->second->nodeName.c_str(), itr->second->nodeId, itr->second->curHeartBeatIndex);

				}
				lck3.unlock();
				//��������ÿ��һ��ʱ��㲥һ�������¼�����ͨ�ڵ������յ������¼��������Ľڵ�ظ�������Ϣ�����Ľڵ���
				//��������ÿ��һ��ʱ����һ����ͨ�ڵ����Ϣ
				heartBeatMsg msg;
				msg.heartBeatIndex = heartBeatIndex;
				boost::posix_time::ptime tnow = boost::posix_time::microsec_clock::universal_time();
				memcpy(&(msg.heartBeatTime), &tnow, 8);

				rpsfPackagePtr p = setSysMsgToPackage(msg, SysMsgType::SYS_HEART_BEAT);
				p->obj.m_handleType = HandleType::RPSF_SYNC;
				///rpsfMsgHandler(p);//ͬ������

				//һ���������������һ��ȫ����Ϣ����
				if ((heartBeatIndex % 2 == 0) && (infoChangeCount<3)) {
					++infoChangeCount;
					//ȫ������һ�����������ٽ���ȫ�����£��������µı�Ķ�����������Ϊ��
					allInfoMsg msg1;
					std::shared_lock<std::shared_mutex> lck(m_mapSysMsgType2NodeIdMutex);
					msg1.mapSysMsgType2NodeId = m_mapSysMsgType2NodeId;
					lck.unlock();
					std::shared_lock<std::shared_mutex> lck1(m_mapDataNmae2NodeIdMutex);
					msg1.mapDataNmae2NodeId = m_mapDataNmae2NodeId;
					lck1.unlock();
					std::shared_lock<std::shared_mutex> lck2(m_mapService2NodeIdMutex);
					msg1.mapService2NodeId = m_mapService2NodeId;
					lck2.unlock();

					rpsfPackagePtr p1 = setSysMsgToPackage(msg1, SysMsgType::SYS_ALL_INFO);
					p1->obj.m_handleType = HandleType::RPSF_UNORDER;
					///rpsfMsgClassifier(p1);//����ͨ������
				}

				++heartBeatIndex;
				
			}
		}


		std::unique_ptr<std::thread> m_pheartBeatThread;
		std::atomic_uint32_t infoChangeCount = 0;
		std::atomic_uint32_t heartBeatIndex = 0;

		std::shared_mutex m_mapNodeInfoMutex;//ֻ������m_mapNodeInfo������ʱʹ��
		std::map<uint32_t, nodeInfoPtr> m_mapNodeInfo;

	};

}


