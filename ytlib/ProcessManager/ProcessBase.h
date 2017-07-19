#pragma once
#include <ytlib/Common/TString.h>
#include <functional>  

namespace ytlib
{
	//��־�ص�
	typedef std::function<void(const tstring &)> LogCallback;
	//���̰߳�ȫ����Ҫ��������
	class ProcessBase
	{
	public:
		ProcessBase():is_init(false), is_running(false){
			registerLogCallback(std::bind(&ProcessBase::defLogCallback, this, std::placeholders::_1));
		}
		virtual ~ProcessBase(){
			if (is_running) stop();
		}

		virtual bool init() {
			is_running = false;
			is_init = true;
			fLogCallback(T_TEXT("Init Successfully"));
			return true;
		}
		virtual bool start() {
			if (!is_init) { 
				fLogCallback(T_TEXT("Start failed : uninitialized"));
				return false; 
			}
			if (is_running) {
				fLogCallback(T_TEXT("Start failed : process is running"));
				return false;
			}
			is_running = true;
			fLogCallback(T_TEXT("Start"));
			return true;
		}
		//pause��ʾ��ͣ�������ٽ���start��stop��ʾֹͣ����������startҲ�Ǵ�ͷ��ʼ
		//�˴���pause��stop������һ���ġ�������������
		virtual bool pause() {
			if (!is_running) {
				fLogCallback(T_TEXT("Pause failed : process is not running"));
				return false;
			}
			is_running = false;
			fLogCallback(T_TEXT("Pause"));
			return true;
		}

		virtual bool stop() {
			if (!is_running) {
				fLogCallback(T_TEXT("Stop failed : process is not running"));
				return false;
			}
			is_running = false;
			fLogCallback(T_TEXT("Stop"));
			return true;
		}

		virtual bool isInit() {
			return is_init;
		}
		virtual bool isRunning() {
			return is_running;
		}
		//������������������ص�ǰ״̬
		virtual int32_t getCurState() {
			return 0;
		}

		inline void registerLogCallback(LogCallback f) {
			fLogCallback = f;
		}

	protected:
		//����־����
		void defLogCallback(const tstring & s) {
			tcout << s << std::endl;
		}
	
		bool is_init;
		bool is_running;
		LogCallback fLogCallback;
		
	};


}