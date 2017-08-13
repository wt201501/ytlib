#pragma once
#include <ytrpsf/RPSF_Interface/IBus.h>

namespace rpsf {
	//���ļ��ṩ�����������

	//�û������Ĳ����Ҫʵ�ִ˽ӿ�
#ifndef CREATE_PLUGIN_STRING
#define CREATE_PLUGIN_STRING CL_TEXT("CreatePlugin")
#endif

	//plugin������ֱ࣬���ṩ���û����п������û���Ҫ�̳���
	class IPlugin {
	public:
		//���ƺͷ�����Ҫ�ڲ������ʱȷ���������Ҳ��ܸı�
		IPlugin(IBus* pBus_, const std::string& name_, const std::set<std::string>& funs_) :
			pBus(pBus_) , name(name_), funs(funs_){	}
		virtual ~IPlugin() {}
		virtual bool Start(std::map<std::string, std::string>& params_) = 0;
		virtual void Stop(void) = 0;
		virtual void OnData(const rpsfData& data_) = 0;
		virtual void OnEvent(const rpsfEvent& event_) = 0;
		virtual rpsfResult Invoke(const rpsfCallArgs& callArgs_) = 0;

		const std::string name;
		const std::set<std::string> funs;
	private:
		IBus* pBus;
	};








}