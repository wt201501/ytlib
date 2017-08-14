#pragma once
#include <map>
#include <set>
#include <boost/shared_array.hpp>

namespace rpsf {
	//���ļ������˲��������֮�佻���Ľӿ�

	//����ʽ
	enum HandleType {
		RPSF_ORDER,//�첽������
		RPSF_UNORDER,//�첽������
		RPSF_SYNC//ͬ������
	};

	//���ߴ���
	enum BusErr {
		RPSF_NO_ERROR=0,
		RPSF_FILE_NOT_EXIST,

		RPSF_SERVICE_NOT_EXIST,
		RPSF_FUN_NOT_EXIST,
		RPSF_TIMEOUT,
		RPSF_RPC_FAILED
	};

	//���ݰ���public�����û����������޸�ֵ�ģ�private�����û�ֻ����
	class rpsfPackage {
	public:

		//��������
		std::map<std::string, std::pair<boost::shared_array<char>, uint32_t>> m_mapDatas_shared_pchar;
		std::map<std::string, std::pair<char*, uint32_t>> m_mapDatas_pchar;
		std::map<std::string, std::string*> m_mapDatas_pstring;

		//�ļ�����
		std::map<std::string, std::string> m_mapFiles;
		bool m_bDelFiles;

		HandleType m_handleType;
	};

	class rpsfData : public rpsfPackage {
	public:
		std::string m_dataName;//��������
		std::string m_sender;//���ݷ�����

		//��ȡ���ݷ���ʱ��
		inline std::string getDataTime() {
			return m_dataTime;
		}
	private:
		std::string m_dataTime;
	};

	class rpsfRpcArgs : public rpsfPackage {
	public:
		rpsfRpcArgs(){}
		rpsfRpcArgs(const std::string& service_, const std::string& fun_) :
			m_service(service_), m_fun(fun_) {}

		std::string m_service;//��������
		std::string m_fun;//��������
	};
	class rpsfRpcResult : public rpsfPackage {
	public:

		BusErr m_rpcErr;
		std::string m_errMsg;
	};

	//���߻���
	class IBus {
	public:
		//���ġ�ȡ���������ݡ��¼�
		virtual BusErr SubscribeData(const std::string& pluginName_, const std::set<std::string>& dataNames_) = 0;
		virtual BusErr UnsubscribeData(const std::string& pluginName_, const std::set<std::string>& dataName_) = 0;
		virtual BusErr SubscribeEvent(const std::string& pluginName_, const std::set<std::string>& eventName_) = 0;
		virtual BusErr UnsubscribeEvent(const std::string& pluginName_, const std::set<std::string>& eventName_) = 0;


		virtual rpsfRpcResult Invoke(const rpsfRpcArgs& callArgs_, uint32_t timeout = 0) = 0;//RPC�ӿڡ�timeout���ȴ�ʱ�䣬0��ʾ��Զ�ȴ�
		virtual BusErr PublishData(const rpsfData& data_) = 0;//��������

		virtual const char* getBusErrMsg(BusErr err) = 0;
	};

	//plugin������ֱ࣬���ṩ���û����п������û���Ҫ�̳���
	class IPlugin {
	public:
		//���ƺͷ�����Ҫ�ڲ������ʱȷ���������Ҳ��ܸı䡣����ָ��Ҳ��Ҫ�ڹ��캯���б����벢��ȷ��
		IPlugin(IBus* pBus_, const std::string& name_, const std::set<std::string>& funs_) :
			pBus(pBus_), name(name_), funs(funs_) {
		}
		virtual ~IPlugin() {}
		virtual bool Start(std::map<std::string, std::string>& params_) = 0;
		virtual void Stop(void) = 0;
		virtual void OnData(const rpsfData& data_) = 0;
		virtual rpsfRpcResult Invoke(const rpsfRpcArgs& callArgs_) = 0;

		const std::string name;
		const std::set<std::string> funs;
	private:
		const IBus* pBus;
	};


	//�û������Ĳ����Ҫʵ�ִ˽ӿ�������һ��IPluginʵ��
#ifndef CREATE_PLUGIN_STRING
#define CREATE_PLUGIN_STRING CL_TEXT("CreatePlugin")
#endif
}