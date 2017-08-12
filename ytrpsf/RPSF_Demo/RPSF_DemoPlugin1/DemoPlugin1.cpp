#include <ytrpsf/RPSF_Demo/RPSF_DemoPlugin1/DemoPlugin1_Client.h>

namespace {

#if defined(_WIN32)
#	define _DECLSPEC_EXPORT __declspec(dllexport)
#else
#	define _DECLSPEC_EXPORT
#endif

	extern "C" _DECLSPEC_EXPORT rpsf::IPlugin* CreatePlugin(rpsf::IBus* pBus_) {
		return new DemoPlugin1(pBus_);
	}



	class DemoPlugin1 : public rpsf::IPlugin {
	public:
		//���ƺͷ�����Ҫ�ڲ������ʱȷ���������Ҳ��ܸı�
		DemoPlugin1(rpsf::IBus* pBus_) :
			rpsf::IPlugin(pBus_, "DemoPlugin1", std::set<std::string>{"testRPC"}) {
		}
		virtual ~DemoPlugin1() {}
		bool Start(std::map<std::string, std::string>& params_) {

		}
		void Stop(void) {

		}
		void OnData(const rpsf::rpsfData& data_) {

		}
		void OnEvent(const rpsf::rpsfEvent& event_) {

		}
		void Invoke(const rpsf::rpsfCallArgs& callArgs_, rpsf::rpsfResult& result_) {
			
		}


	private:
		//����ģ���麯����

	};



}