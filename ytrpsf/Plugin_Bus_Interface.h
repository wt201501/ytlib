#pragma once
#include <map>
#include <set>
#include <ytlib/NetTools/SharedBuf.h>
#include <memory>

namespace rpsf {
	//���ļ������˲��������֮�佻���Ľӿ�

	//����ʽ
	enum HandleType {
		RPSF_ORDER=0,//�첽������
		RPSF_UNORDER=1,//�첽������
		RPSF_SYNC=2//ͬ������
	};

	//���ߴ���
	//BusErr����С��256��.��Ϊ���ǿ�ƽ̨ʱ�Ĵ�С������
	enum BusErr {
		RPSF_NO_ERROR=0,
		RPSF_FILE_NOT_EXIST,

		RPSF_SERVICE_NOT_EXIST,
		RPSF_TIMEOUT,
		RPSF_RPC_FAILED,

		RPSF_SUBSCRIBE_DATA_FAILED,
		RPSF_UNSUBSCRIBE_DATA_FAILED,

		RPSF_SUBSCRIBE_SERVICE_FAILED,
		RPSF_UNSUBSCRIBE_SERVICE_FAILED,

		RPSF_ERR_COUNT
	};

	//���ݰ���ע�⣺�첽�������ߵĽӿڵģ���Ҫ�ڵ������߽ӿ�֮��ȥ�޸����ݡ���������ɾ�������һ�����ݣ�����������ݣ��������޸�һ�����ݵľ�������
	class rpsfPackage {
	public:

		//���ָ��Ʒ�ʽû�и������ݣ���ȷ��buf_�е����ݲ���仯
		void rpsfPackage::addData_unsafe(const std::string& dataTag, const boost::shared_array<char>& buf_, uint32_t buf_size_) {
			m_mapDatas[dataTag] = std::move(ytlib::sharedBuf(buf_, buf_size_));
		}

		void rpsfPackage::addData(const std::string& dataTag, const char* buf_, uint32_t buf_size_) {
			m_mapDatas[dataTag] = std::move(ytlib::sharedBuf(buf_, buf_size_));
		}
		void rpsfPackage::addData(const std::string& dataTag, const std::string& data_) {
			m_mapDatas[dataTag] = std::move(ytlib::sharedBuf(data_));
		}

		//��������
		std::map<std::string, ytlib::sharedBuf > m_mapDatas;
		//�ļ�����
		std::map<std::string, std::string> m_mapFiles;
		bool m_bDelFiles;

		HandleType m_handleType;
	};

	class rpsfData : public rpsfPackage {
	public:
		rpsfData() {}
		rpsfData(const std::string& dataName_) :m_dataName(dataName_) {	}
		std::string m_dataName;//��������
	};

	class rpsfRpcArgs : public rpsfPackage {
	public:
		rpsfRpcArgs(){}
		rpsfRpcArgs(const std::string& service_) :m_service(service_){}
		std::string m_service;//��������
	};
	class rpsfRpcResult : public rpsfPackage {
	public:
		rpsfRpcResult(){}
		rpsfRpcResult(BusErr err):m_rpcErr(err){}

		BusErr m_rpcErr;
		std::string m_errMsg;
	};
	
	//���߻���
	class IPlugin;
	class IBus {
	public:
		IBus(){}
		virtual ~IBus(){}
		//���ġ�ȡ���������ݡ�����
		virtual BusErr SubscribeData(const IPlugin* pPlugin_, const std::string& dataNames_) = 0;
		virtual BusErr UnsubscribeData(const IPlugin* pPlugin_, const std::string& dataName_) = 0;
		virtual std::set<std::string> GetSubscribeDataList(const IPlugin* pPlugin_) = 0;

		virtual BusErr SubscribeService(const IPlugin* pPlugin_, const std::string& service_, const std::string& remark_="") = 0;
		virtual BusErr UnsubscribeService(const IPlugin* pPlugin_, const std::string& service_) = 0;
		virtual std::map<std::string, std::string> GetSubscribeServiceList(const IPlugin* pPlugin_) = 0;

		//���ݴ����ʧЧ
		//���������ϵķ�������ʽ��
		virtual rpsfRpcResult Invoke(rpsfRpcArgs& callArgs_, uint32_t timeout = 0) = 0;//RPC�ӿڡ�timeout���ȴ�ʱ�䣬0��ʾ��Զ�ȴ�
		//�������Ϸ�������
		virtual BusErr PublishData(rpsfData& data_) = 0;//��������

		virtual const char* getBusErrMsg(BusErr err) = 0;
	};

	//�����������û������ģ�ʵ��һЩ�ӿڵı������õĶ���ģ��
	//plugin������ֱ࣬���ṩ���û����п������û���Ҫ�̳���
	class IPlugin {
	public:
		//���ƺͷ�����Ҫ�ڲ������ʱȷ���������Ҳ��ܸı䡣����ָ��Ҳ��Ҫ�ڹ��캯���б����벢��ȷ��
		IPlugin(IBus* pBus_, const std::string& name_) :
			pBus(pBus_), name(name_) {
		}
		virtual ~IPlugin() {}
		virtual bool Start(std::map<std::string, std::string>& params_) = 0;
		virtual void Stop(void) = 0;
		virtual void OnData(const rpsfData& data_) = 0;
		virtual rpsfRpcResult Invoke(rpsfRpcArgs& callArgs_) = 0;

		//�����߽ӿڵĿ�ݲ���
		inline BusErr SubscribeData(const std::string& dataNames_) {
			return pBus->SubscribeData(this, dataNames_);
		}

		inline BusErr UnsubscribeData(const std::string& dataNames_) {
			return pBus->UnsubscribeData(this, dataNames_);
		}
		inline std::set<std::string> GetSubscribeDataList() {
			return pBus->GetSubscribeDataList(this);
		}

		inline BusErr SubscribeService(const std::string& service_, const std::string& remark_ = "") {
			return pBus->SubscribeService(this, service_, remark_);
		}
		inline BusErr UnsubscribeService(const std::string& service_) {
			return pBus->UnsubscribeService(this, service_);
		}
		inline std::map<std::string, std::string> GetSubscribeServiceList() {
			return pBus->GetSubscribeServiceList(this);
		}


		const std::string name;
	protected:
		IBus* pBus;
	};


	//�û������Ĳ����Ҫʵ�ִ˽ӿ�������һ��IPluginʵ��
#ifndef CREATE_PLUGIN_STRING
#define CREATE_PLUGIN_STRING CL_TEXT("CreatePlugin")
#endif

}