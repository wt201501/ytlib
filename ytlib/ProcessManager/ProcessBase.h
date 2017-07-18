#pragma once
#include <ytlib/Common/TString.h>
#include <functional>  
//#include <mutex>

namespace wtlib
{
	//日志回调
	typedef std::function<void(const tstring &)> LogCallback;
	//非线程安全，不要并发操作
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
			fLogCallback(WT_TEXT("Init Successfully"));
			return true;
		}
		virtual bool start() {
			if (!is_init) { 
				fLogCallback(WT_TEXT("Start failed : uninitialized"));
				return false; 
			}
			if (is_running) {
				fLogCallback(WT_TEXT("Start failed : process is running"));
				return false;
			}
			is_running = true;
			fLogCallback(WT_TEXT("Start"));
			return true;
		}
		//pause表示暂停，可以再接着start。stop表示停止，就算重新start也是从头开始
		//此处的pause和stop表现是一样的。交给子类重载
		virtual bool pause() {
			if (!is_running) {
				fLogCallback(WT_TEXT("Pause failed : process is not running"));
				return false;
			}
			is_running = false;
			fLogCallback(WT_TEXT("Pause"));
			return true;
		}

		virtual bool stop() {
			if (!is_running) {
				fLogCallback(WT_TEXT("Stop failed : process is not running"));
				return false;
			}
			is_running = false;
			fLogCallback(WT_TEXT("Stop"));
			return true;
		}

		virtual bool isInit() {
			return is_init;
		}
		virtual bool isRunning() {
			return is_running;
		}
		//由派生类决定怎样返回当前状态
		virtual int32_t getCurState() {
			return 0;
		}

		inline void registerLogCallback(LogCallback f) {
			fLogCallback = f;
		}

	protected:
		//简单日志功能
		void defLogCallback(const tstring & s) {
			tcout << s << std::endl;
		}
	
		bool is_init;
		bool is_running;
		LogCallback fLogCallback;
		
	};


}