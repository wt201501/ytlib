#pragma once
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

	//��ͨ�ڵ�����ߡ����Ľڵ�����߼̳���ͨ�ڵ������
	class Bus : public IBus {
	public:
		
		Bus(){}
		virtual ~Bus() {}
		bool Init(uint32_t NodeId_, uint16_t NodePort_, ytlib::tstring recvpath_, ytlib::tstring sendpath_) {

			//����ͨ��
			m_orderChannel = std::make_shared<ytlib::ChannelBase<rpsfPackagePtr> >(std::bind(&Bus::rpsfMsgHandler, this, std::placeholders::_1));
			m_unorderChannel = std::make_shared<ytlib::ChannelBase<rpsfPackagePtr> >(std::bind(&Bus::rpsfMsgHandler, this, std::placeholders::_1), 5);
		
			//�����ܿ�����������������
			m_netAdapter = std::make_shared<ytlib::TcpNetAdapter<rpsfMsg> >(NodeId_, NodePort_, std::bind(&Bus::on_RecvCallBack, this, std::placeholders::_1), recvpath_, sendpath_);
		
			if(BusTrivialConfig(thisnode)) return false;
			
			if (!(m_netAdapter->start())) return false;


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
		//�������������ʵ�ֲ�ͬ���͵�����
		virtual bool BusTrivialConfig(const RpsfNode& cfg_){
			//common node
			m_netAdapter->SetHost(cfg_.CenterNodeId, cfg_.CenterNodeIp, cfg_.CenterNodePort);

			return true;
		}

		//�յ���Ϣ�Ļص�
		void on_RecvCallBack(rpsfPackagePtr& pmsg) {

		}

		//������Ϣ
		void rpsfMsgHandler(rpsfPackagePtr& pmsg) {

		}

		//���г�Ա��ֻ��������ָ����
		std::shared_ptr<ytlib::TcpNetAdapter<rpsfMsg> >  m_netAdapter;//����������
		std::shared_ptr<ytlib::ChannelBase<rpsfPackagePtr> > m_orderChannel;//����ͨ��
		std::shared_ptr<ytlib::ChannelBase<rpsfPackagePtr> > m_unorderChannel;//����ͨ��

	};

#define PBUS(p) static_cast<Bus*>(p)

}