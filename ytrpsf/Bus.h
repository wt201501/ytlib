#pragma once
#include <ytrpsf/Client_Bus_Interface.h>
#include <ytrpsf/RpsfCfgFile.h>
#include <ytrpsf/Msg.h>
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

	class Bus : public IBus {
	public:
		//�ӿڣ�bus-client
		Bus():m_bInit(false), thisNodeType(RpsfNodeType::NODETYPE_COMMON){

		}
		virtual ~Bus() {

		}
		bool Init(const std::string& cfgpath) {
			RpsfCfgFile cfgfile;
			try {
				cfgfile.OpenFile(T_STRING_TO_TSTRING(cfgpath));
			}
			catch (const ytlib::Exception& e) {
				std::cout << e.what() << std::endl;
				return false;
			}
			RpsfNode& thisnode = *(cfgfile.m_fileobj.get());
			if (thisnode.NodeType != thisNodeType) {
				tcout << T_TEXT("error : node type mismatch! please check the cfg file and exe type.") << std::endl;
				return false;
			}


			//����ͨ��
			m_orderChannel = std::make_shared<ytlib::ChannelBase<rpsfPackagePtr> >(std::bind(&Bus::rpsfMsgHandler, this, std::placeholders::_1));
			m_unorderChannel = std::make_shared<ytlib::ChannelBase<rpsfPackagePtr> >(std::bind(&Bus::rpsfMsgHandler, this, std::placeholders::_1), 5);
		


			//�����ܿ�����������������
			//�����ļ��շ�·��
			ytlib::tstring nodename(T_STRING_TO_TSTRING(thisnode.NodeName));
			ytlib::tstring recvpath(nodename + T_TEXT("/recv"));
			ytlib::tstring sendpath(nodename + T_TEXT("/send"));
			std::map<ytlib::tstring, ytlib::tstring>::iterator itr = thisnode.NodeSettings.find(T_TEXT("recvpath"));
			if (itr != thisnode.NodeSettings.end()) recvpath = itr->second;
			itr = thisnode.NodeSettings.find(T_TEXT("sendpath"));
			if (itr != thisnode.NodeSettings.end()) sendpath = itr->second;

			m_netAdapter = std::make_shared<ytlib::TcpNetAdapter<rpsfMsg> >(thisnode.NodeId, thisnode.NodePort, std::bind(&Bus::on_RecvCallBack, this, std::placeholders::_1), recvpath, sendpath);
		
			if(BusTrivialConfig(thisnode)) return false;
			
			m_bInit = m_netAdapter->start();
		}

		bool AddPlugin(IPlugin* p_plugin) {

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
		rpsfRpcResult Invoke(std::unique_ptr<rpsfRpcArgs>& callArgs_, uint32_t timeout = 0) {

		}
		BusErr PublishData(std::unique_ptr<rpsfData>& data_) {

		}

		const char* getBusErrMsg(BusErr err) {
			return busErrMsg[err];
		}
		
	protected:
		//�������������ʵ�ֲ�ͬ���͵�����
		virtual bool BusTrivialConfig(const RpsfNode& cfg_){
			//common node
			m_netAdapter->SetHost(cfg_.CenterNodeId, cfg_.CenterNodeIp, cfg_.CenterNodePort);
		}


		void on_RecvCallBack(rpsfPackagePtr& pmsg) {

		}

		void rpsfMsgHandler(rpsfPackagePtr& pmsg) {

		}



		//���г�Ա��ֻ��������ָ����
		bool m_bInit;
		RpsfNodeType thisNodeType;

		std::shared_ptr<ytlib::TcpNetAdapter<rpsfMsg> >  m_netAdapter;//����������
		std::shared_ptr<ytlib::ChannelBase<rpsfPackagePtr> > m_orderChannel;//����ͨ��
		std::shared_ptr<ytlib::ChannelBase<rpsfPackagePtr> > m_unorderChannel;//����ͨ��

	};

#define PBUS(p) static_cast<Bus*>(p)

	Client::Client() {
		p = new Bus();
	}
	Client::~Client() {
		delete p;
	}
	bool Client::Init(const std::string& cfgpath) {
		return PBUS(p)->Init(cfgpath);
	}

	bool Client::AddPlugin(IPlugin* p_plugin) {
		return PBUS(p)->AddPlugin(p_plugin);
	}

	IBus* Client::get_pIBus() {
		return PBUS(p);
	}
}