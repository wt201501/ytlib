#pragma once
#include <ytrpsf/SysMsg.h>
#include <ytrpsf/RpsfCfgFile.h>
#include <ytlib/LogService/Logger.h>
#include <ytlib/SupportTools/ChannelBase.h>
#include <ytlib/SupportTools/DynamicLibraryContainer.h>


namespace rpsf {

	static const char* busErrMsg[RPSF_ERR_COUNT] =
	{
		"successful.",
		"file not exist.",
		"service not exist."
		"RPC timeout.",
		"RPC failed.",
		"subscribe data failed",
		"unsubscribe data failed",
		"subscribe service failed",
		"unsubscribe service failed"
	};

	//���ߡ�һЩͨ�õĲ������ڴ˴����
	class Bus : public IBus {
	public:
		
		Bus() :m_bRunning(false){}
		virtual ~Bus() { Stop(); }

		virtual bool Init(const RpsfNode& thisnode_) {
			m_NodeId = thisnode_.NodeId;
			//��ʼ����־
			if (thisnode_.UseNetLog) {
				ytlib::InitNetLog(thisnode_.NodeId, ytlib::TcpEp(boost::asio::ip::address::from_string(thisnode_.LogServerIp), thisnode_.LogServerPort));
			}

			//��ʼ��������


			//����ͨ��
			m_orderChannel = std::make_shared<ytlib::ChannelBase<rpsfPackagePtr> >(std::bind(&Bus::rpsfMsgHandler, this, std::placeholders::_1));
			m_unorderChannel = std::make_shared<ytlib::ChannelBase<rpsfPackagePtr> >(std::bind(&Bus::rpsfMsgHandler, this, std::placeholders::_1), 5);
		

			//�����ļ��շ�·��
			ytlib::tstring nodename(T_STRING_TO_TSTRING(thisnode_.NodeName));
			ytlib::tstring recvpath(nodename + T_TEXT("/recv"));
			ytlib::tstring sendpath(nodename + T_TEXT("/send"));
			std::map<ytlib::tstring, ytlib::tstring>::const_iterator itr = thisnode_.NodeSettings.find(T_TEXT("recvpath"));
			if (itr != thisnode_.NodeSettings.end()) recvpath = itr->second;
			itr = thisnode_.NodeSettings.find(T_TEXT("sendpath"));
			if (itr != thisnode_.NodeSettings.end()) sendpath = itr->second;
			//�����ܿ�����������������
			m_netAdapter = std::make_shared<ytlib::TcpNetAdapter<rpsfMsg> >(thisnode_.NodeId, thisnode_.NodePort, std::bind(&Bus::rpsfMsgClassifier, this, std::placeholders::_1), recvpath, sendpath);

			if (!(m_netAdapter->start())) return false;


			return true;

		}

		virtual bool Stop() {
			if (!m_bRunning) return true;//�Ѿ�ֹͣ��
			m_bRunning = false;


			m_orderChannel->Stop();
			m_unorderChannel->Stop();
			m_netAdapter->stop();
			ytlib::StopNetLog();

			return !m_bRunning;
		}

		//�ӿڣ�bus-plugin
		BusErr SubscribeData(const IPlugin* pPlugin_, const std::string& dataNames_) {

			return BusErr::RPSF_NO_ERROR;
		}
		BusErr UnsubscribeData(const IPlugin* pPlugin_, const std::string& dataName_) {

			return BusErr::RPSF_NO_ERROR;
		}
		std::set<std::string> GetSubscribeDataList(const IPlugin* pPlugin_) {

			return std::set<std::string>();
		}

		BusErr SubscribeService(const IPlugin* pPlugin_, const std::string& service_, const std::string& remark_ = "") {

			return BusErr::RPSF_NO_ERROR;
		}
		BusErr UnsubscribeService(const IPlugin* pPlugin_, const std::string& service_) {

			return BusErr::RPSF_NO_ERROR;
		}
		std::map<std::string, std::string> GetSubscribeServiceList(const IPlugin* pPlugin_) {

			return std::map<std::string, std::string>();
		}

		rpsfRpcResult Invoke(rpsfRpcArgs& callArgs_, uint32_t timeout = 0) {

			return rpsfRpcResult();
		}

		BusErr PublishData(rpsfData& data_) {
			rpsfMsgClassifier(setMsgToPackage(data_));

			return BusErr::RPSF_NO_ERROR;
		}

		const char* getBusErrMsg(BusErr err) {
			return busErrMsg[err];
		}
		
	protected:
		//���ض���ϵͳ�¼�
		virtual void SubscribeSysEvent(const std::set<SysMsgType>& sysEvents_) {
			std::unique_lock<std::shared_mutex> lck(m_mapSysMsgType2NodeIdMutex);
			for (std::set<SysMsgType>::const_iterator itr = sysEvents_.begin(); itr != sysEvents_.end(); ++itr) {
				m_mapSysMsgType2NodeId[*itr].insert(m_NodeId);
			}

		}
		//����ȡ������ϵͳ�¼�
		virtual void UnsubscribeSysEvent(const std::set<SysMsgType>& sysEvents_) {
			std::unique_lock<std::shared_mutex> lck(m_mapSysMsgType2NodeIdMutex);
			for (std::set<SysMsgType>::const_iterator itr = sysEvents_.begin(); itr != sysEvents_.end(); ++itr) {
				std::map<SysMsgType, std::set<uint32_t> >::iterator itr2 = m_mapSysMsgType2NodeId.find(*itr);
				if (itr2 == m_mapSysMsgType2NodeId.end()) continue;
				std::set<uint32_t>::iterator itr3 = itr2->second.find(m_NodeId);
				if (itr3 == itr2->second.end()) continue;
				itr2->second.erase(itr3);
				if (itr2->second.empty()) m_mapSysMsgType2NodeId.erase(itr2);
			}
		}

		//���ز��
		virtual bool rpsfLoadOnePlugin(const std::string& pgname_, bool enable_, const std::map<std::string, std::string>& initParas) {
			std::pair<std::shared_ptr<ytlib::DynamicLibrary>, bool> result = GET_LIB(T_STRING_TO_TSTRING(pgname_));
			if (!result.first) {
				//û������
				return false;
			}
			if (result.second) {
				//�Ѿ����ع���
				return false;
			}
			typedef IPlugin* (*CreatePluginFun)(IBus*);
			CreatePluginFun CreatePlugin = (CreatePluginFun)result.first->GetSymbol(CREATE_PLUGIN_STRING);
			if (CreatePlugin) {
				IPlugin* pPlugin = CreatePlugin(this);//�����������
				if (pPlugin == NULL) {
					//����ʧ��
					return false;
				}
				if (pgname_ != pPlugin->name) {
					//���Ʋ���Ӧ
					return false;
				}
				pPlugin->Start(initParas);
				std::unique_lock<std::shared_mutex> lck(m_mapPgName2PgPointMutex);
				m_mapPgName2PgPoint.insert(std::pair<std::string, std::pair<IPlugin*, bool> >(pgname_, std::pair<IPlugin*, bool>(pPlugin, enable_)));
				//���سɹ�
			}
			return true;
		}
		//�������ز��
		virtual void rpsfLoadPlugins(const std::set<PluginCfg>& plugins) {
			for (std::set<PluginCfg>::const_iterator itr = plugins.begin(); itr != plugins.end(); ++itr) {
				rpsfLoadOnePlugin(itr->PluginName, itr->enable, itr->InitParas);
			}
		}

		//ж�ز��
		virtual bool rpsfRemoveOnePlugin(const std::string& pgname_) {
			//��ʧ�ܴ˲��
			rpsfEnableOnePlugin(pgname_, false);
			//ȡ����˲���йص�һ�ж��ġ�RPC
			std::unique_lock<std::shared_mutex> lck(m_mapPgName2PgPointMutex);
			std::map<std::string, std::pair<IPlugin*, bool> >::iterator itr = m_mapPgName2PgPoint.find(pgname_);
			if (itr == m_mapPgName2PgPoint.end()) {
				//��Ҫж��δ���صĲ��
				return false;
			}

			
			itr->second.first->Stop();//ֹͣ���
			m_mapPgName2PgPoint.erase(itr);//�ӱ���ɾ��
			//��ж�ش˲��
			if (!REMOVE_LIB(T_STRING_TO_TSTRING(pgname_))) {
				//ж��ʧ��
				return false;
			}
			return true;
		}
		//�����ʹ�ܵ���˼�ǣ������Ȼ�����У����������ݡ�RPC����ʱ�������
		//ʹ�ܲ��
		virtual bool rpsfEnableOnePlugin(const std::string& pgname_, bool enable_ = true) {
			std::unique_lock<std::shared_mutex> lck(m_mapPgName2PgPointMutex);
			std::map<std::string, std::pair<IPlugin*, bool> >::iterator itr = m_mapPgName2PgPoint.find(pgname_);
			if (itr == m_mapPgName2PgPoint.end()) {
				//û�д˲��
				return false;
			}
			itr->second.second = enable_;
			return true;
		}

		//��Ϣ���������������յ���Ϣ�Ļص�
		void rpsfMsgClassifier(rpsfPackagePtr& pmsg) {
			switch (pmsg->obj.m_handleType) {
			case HandleType::RPSF_SYNC :
				rpsfMsgHandler(pmsg);
				break;
			case HandleType::RPSF_ORDER:
				m_orderChannel->Add(pmsg);
				break;
			case HandleType::RPSF_UNORDER:
				m_unorderChannel->Add(pmsg);
				break;
			default:
				break;
			}

		}

		//������Ϣ
		void rpsfMsgHandler(rpsfPackagePtr& pmsg) {
			//���ж��Ƿ��Ǳ��ط�����
			if (pmsg->obj.m_srcAddr == 0) {
				pmsg->obj.m_srcAddr = m_NodeId;
				//����Ǳ��صģ��ҳ�Ŀ�ĵأ����Ŀ�ĵ��������ڵ㣬��ͨ�����緢��
				std::set<uint32_t> dst = std::move(pmsg->obj.m_pushList);//��ȡ���͵�ַ

				//�����������ͣ��Ӳ�ͬ�ı�����Ŀ�ĵ�
				switch (pmsg->obj.m_msgType) {
				case MsgType::RPSF_SYS: {
					//������Щ�ڵ㶩���˴�ϵͳ�¼�
					std::shared_lock<std::shared_mutex> lck(m_mapSysMsgType2NodeIdMutex);
					std::map<SysMsgType, std::set<uint32_t> >::const_iterator itr =	m_mapSysMsgType2NodeId.find(getSysMsgTypeFromPackage(pmsg));
					if (itr != m_mapSysMsgType2NodeId.end()) dst.insert(itr->second.begin(), itr->second.end());
					break;
				}
				case MsgType::RPSF_DATA: {
					//������Щ�ڵ㶩���˴�����
					std::shared_lock<std::shared_mutex> lck(m_mapDataNmae2NodeIdMutex);
					std::map<std::string, std::set<uint32_t> >::const_iterator itr = m_mapDataNmae2NodeId.find(getDataNameFromPackage(pmsg));
					if (itr != m_mapDataNmae2NodeId.end()) dst.insert(itr->second.begin(), itr->second.end());
					break;
				}
				case MsgType::RPSF_RPC: {
					//������Щ�ڵ��ṩ��RPC����
					std::shared_lock<std::shared_mutex> lck(m_mapService2NodeIdMutex);
					std::map<std::string, std::set<uint32_t> >::const_iterator itr = m_mapService2NodeId.find(getServiceFromPackage(pmsg));
					if (itr != m_mapService2NodeId.end()) dst.insert(itr->second.begin(), itr->second.end());
					break;
				}
				case MsgType::RPSF_RRPC: {
					//�����RPC�����Ǵ��ĸ��ڵ㷢������
					dst.insert(getRrpcDstFromPackage(pmsg));
					break;
				}
				default:
					break;
				}

				//���Ŀ�ĵ��Ǳ��أ�����д���
				std::set<uint32_t>::const_iterator itr = dst.find(m_NodeId);
				if (itr != dst.end()) {
					rpsfMsgHandlerLocal(pmsg);
					dst.erase(itr);
				}

				//�Ƚ��б��ش����ٷ��͵�����
				if (dst.size() > 0)	m_netAdapter->Send(pmsg, dst, pmsg->obj.m_delfiles);
			}
			else {
				//����Ǵ������յ���
				rpsfMsgHandlerLocal(pmsg);
			}
		}

		void rpsfMsgHandlerLocal(rpsfPackagePtr& pmsg){
			//���Ŀ�ĵ��Ǳ��ؽڵ㣬������Ϣ�Ǵ��������յ��ģ�Ŀ�ĵؿ϶��Ǳ��أ�������д���
			switch (pmsg->obj.m_msgType) {
			case MsgType::RPSF_SYS: {
				rpsfSysMsg sysMsg;
				getMsgFromPackage(pmsg, sysMsg);
				rpsfSysHandler(sysMsg);
				break;
			}
			case MsgType::RPSF_DATA: {
				rpsfData data;
				getMsgFromPackage(pmsg, data);
				//�ҵ����ض�Ӧ�Ĳ�����в��л��첽�ɷ����Լ��ٴ˲���ռ�ò���б��ʱ��
				std::shared_lock<std::shared_mutex> lck(m_mapDataName2PgNameMutex);
				std::map<std::string, std::set<std::string> >::const_iterator itr = m_mapDataName2PgName.find(data.m_dataName);
				if (itr == m_mapDataName2PgName.end())	break;
				std::vector<std::thread> onDataThreads;
				std::shared_lock<std::shared_mutex> lck2(m_mapPgName2PgPointMutex);
				for (std::set<std::string>::const_iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2) {
					std::map<std::string, std::pair<IPlugin*, bool> >::const_iterator itr3 = m_mapPgName2PgPoint.find(*itr2);
					if ((itr3 != m_mapPgName2PgPoint.end())&&(itr3->second.second)) {
						//�����̡߳�todo:��Ҫ����Ҫ��Ҫstd::move?
						onDataThreads.push_back(std::thread(std::bind(&IPlugin::OnData, itr3->second.first, std::placeholders::_1), data));
					}
				}
				lck2.unlock();
				lck.unlock();
				
				//�ȴ��߳̽���
				size_t len = onDataThreads.size();
				for (size_t ii = 0; ii < len; ++ii) {
					onDataThreads[ii].join();
				}

				break;
			}	
			case MsgType::RPSF_RPC: {
				rpsfRpcArgs rpcArgs;
				getMsgFromPackage(pmsg, rpcArgs);
				//�ҵ����صĲ�����е���

				//�������������ɷ���������

				break;
			}
			case MsgType::RPSF_RRPC: {
				rpsfRpcResult rpcResult;
				getMsgFromPackage(pmsg, rpcResult);

				break;
			}
			default:
				break;
			}
			
		}

		//ϵͳ��Ϣ����ͨ�õ�һЩϵͳ�¼��ڴ˴δ��������Ľ����ϲ�
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

				break;
			}

		}


		//���г�Ա��ֻ��������ָ����
		std::shared_ptr<ytlib::TcpNetAdapter<rpsfMsg> >  m_netAdapter;//����������
		std::shared_ptr<ytlib::ChannelBase<rpsfPackagePtr> > m_orderChannel;//����ͨ��
		std::shared_ptr<ytlib::ChannelBase<rpsfPackagePtr> > m_unorderChannel;//����ͨ��

		bool m_bRunning;

		uint32_t m_NodeId;

		//ϵͳ�¼���ڵ�id�ı�
		std::shared_mutex m_mapSysMsgType2NodeIdMutex;
		std::map<SysMsgType, std::set<uint32_t> > m_mapSysMsgType2NodeId;
		//����������ڵ�id�ı�
		std::shared_mutex m_mapDataNmae2NodeIdMutex;
		std::map<std::string, std::set<uint32_t> > m_mapDataNmae2NodeId;
		//����������ڵ�id�ı�
		std::shared_mutex m_mapService2NodeIdMutex;
		std::map<std::string, std::set<uint32_t> > m_mapService2NodeId;

		//�����������ָ��ı�
		std::shared_mutex m_mapPgName2PgPointMutex;
		std::map<std::string, std::pair<IPlugin*,bool> > m_mapPgName2PgPoint;

		//���ز�����ĵ����������������Ƶı�
		std::shared_mutex m_mapDataName2PgNameMutex;
		std::map<std::string, std::set<std::string> > m_mapDataName2PgName;

	};


}