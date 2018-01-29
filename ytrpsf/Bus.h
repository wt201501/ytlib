#pragma once
#include <ytrpsf/Msg.h>
#include <ytrpsf/RpsfCfgFile.h>
#include <ytlib/LogService/Logger.h>
#include <ytlib/SupportTools/ChannelBase.h>


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
		
		Bus() :m_bInit(false), m_bRunning(false) {}
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

			

			return true;

		}

		virtual bool Start() {
			if (!m_bInit)return false;


			if (!(m_netAdapter->start())) return false;


			m_bRunning = true;
			return m_bRunning;
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

		}
		BusErr UnsubscribeData(const IPlugin* pPlugin_, const std::string& dataName_) {

		}
		std::set<std::string> GetSubscribeDataList(const IPlugin* pPlugin_) {

		}

		BusErr SubscribeService(const IPlugin* pPlugin_, const std::string& service_, const std::string& remark_ = "") {

		}
		BusErr UnsubscribeService(const IPlugin* pPlugin_, const std::string& service_) {

		}
		std::map<std::string, std::string> GetSubscribeServiceList(const IPlugin* pPlugin_) {

		}
		rpsfRpcResult Invoke(rpsfRpcArgs& callArgs_, uint32_t timeout = 0) {

		}
		BusErr PublishData(rpsfData& data_) {

		}

		const char* getBusErrMsg(BusErr err) {
			return busErrMsg[err];
		}
		
	protected:

		//�������յ���Ϣ�Ļص�
		void rpsfMsgClassifier(rpsfPackagePtr& pmsg) {
			switch (pmsg->obj.m_handleType) {
			case HandleType::RPSF_SYNC :
				rpsfMsgHandlerLocal(pmsg);
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
				std::set<uint32_t> dst;

				//�����������ͣ��Ӳ�ͬ�ı�����Ŀ�ĵ�
				switch (pmsg->obj.m_msgType) {
				case MsgType::RPSF_SYS: {
					//������Щ�ڵ㶩���˴�ϵͳ�¼�
					std::shared_lock<std::shared_mutex> lck(m_mapSysMsgType2NodeIdMutex);
					std::map<SysMsgType, std::set<uint32_t> >::const_iterator itr =	m_mapSysMsgType2NodeId.find(getSysMsgTypeFromPackage(pmsg));
					if (itr != m_mapSysMsgType2NodeId.end()) dst = itr->second;
					break;
				}
				case MsgType::RPSF_DATA: {
					//������Щ�ڵ㶩���˴�����
					std::shared_lock<std::shared_mutex> lck(m_mapDataNmae2NodeIdMutex);
					std::map<std::string, std::set<uint32_t> >::const_iterator itr = m_mapDataNmae2NodeId.find(getDataNameFromPackage(pmsg));
					if (itr != m_mapDataNmae2NodeId.end()) dst = itr->second;
					break;
				}
				case MsgType::RPSF_RPC: {
					//������Щ�ڵ��ṩ��RPC����
					std::shared_lock<std::shared_mutex> lck(m_mapService2NodeIdMutex);
					std::map<std::string, std::set<uint32_t> >::const_iterator itr = m_mapService2NodeId.find(getServiceFromPackage(pmsg));
					if (itr != m_mapService2NodeId.end()) dst = itr->second;
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
				//�ҵ����ض�Ӧ�Ĳ�������ɷ�

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
		void rpsfSysHandler(const rpsfSysMsg& m_) {
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

		bool m_bInit;
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



	};


}