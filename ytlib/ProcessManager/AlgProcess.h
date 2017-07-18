#pragma once
#include <ytlib/ProcessManager/ProcessBase.h>
#include <sstream>
#include <map>
#include <boost/thread.hpp>

namespace wtlib
{
	enum AlgRunType
	{
		ALG_SYNC,
		ALG_ASYNC
	};

	//���Ȼص�
	typedef std::function<void(int32_t)> ScheCallback;
	//�㷨��Process������һ���߳��������㷨����
	class AlgProcess : public ProcessBase
	{
	public:
		AlgProcess() :ProcessBase(), m_state(0){
			registerScheCallback(std::bind(&AlgProcess::defScheCallback, this, std::placeholders::_1));
		}
		virtual ~AlgProcess() {
			if(is_running) stop();
		}
		bool init(const std::map<tstring, tstring>& m) {
			m_mapFiles = m;
			return ProcessBase::init();
		}
		bool init() {
			return ProcessBase::init();
		}

		//��Ҫ��ʾ�����ã�Ĭ���첽����
		bool start(AlgRunType type_= AlgRunType::ALG_ASYNC) {
			if (!ProcessBase::start()) return false;
			if (type_ == AlgRunType::ALG_ASYNC) {
				mainAlgThread = boost::thread(std::bind(&AlgProcess::mainAlg, this));
			}
			else {
				mainAlg();
			}
			return true;
		}
		//�����첽ģʽ������
		bool stop(int32_t waittime=1000) {
			if (!ProcessBase::stop()) return false;
			//�ȴ� waittime ms����������оͷ���false
			if (!mainAlgThread.timed_join(boost::posix_time::millisec(waittime))){
				fLogCallback(WT_TEXT("�㷨�����޷�ֹͣ��"));
				return false;
			}
			return true;
		}

		bool isRunning() {
			if (!is_running) return false;
			if (mainAlgThread.timed_join(boost::posix_time::millisec(0))) {
				is_running = false;
			}
			return is_running;
		}

		inline int32_t getCurState() {
			return m_state;
		}

		inline void registerScheCallback(ScheCallback f) {
			fScheCallback = f;
		}
		bool setFiles(const tstring& filename, const tstring&  filepath){
			if (is_running) {
				fLogCallback(WT_TEXT("�㷨�������У��޷������ļ�"));
				return false;
			}
			m_mapFiles[filename] = filepath;
			return true;
		}
		inline std::map<tstring, tstring> getFiles() const{
			return m_mapFiles;
		}

	protected:
		//��ʱ���ṩ��ͣ����
		bool pause(){}

		void defScheCallback(int32_t s) {
			tostringstream ss;
			ss << WT_TEXT("��ǰ���ȣ�") << s;
			fLogCallback(ss.str());
		}
		ScheCallback fScheCallback;

		//״ֵ̬
		int32_t m_state;
		//�����ļ�������+Ŀ¼
		std::map<tstring, tstring> m_mapFiles;//����Ĳ����ļ�·��������ļ�·��һ��
		//�߳�
		boost::thread mainAlgThread;
		
		//ֻ��Ҫ�̳д˷���������
		//��ΪҪ�ṩ���ڽӿ�֧�֣����Բ��ܸĳɺ���ע����ʽ
		//ʾ����
		//һ��Ҫ�ڽ���ʱ��is_running����Ϊfalse
		//����ṩ��⵽is_running��Ϊfalse��ֹͣ�Ĺ���
		//����ֵ�ᱻgetCurState�������ظ��ϲ������
		virtual void mainAlg() {
			fLogCallback(WT_TEXT("����"));
			int32_t ii = 0;
			while (is_running&&(ii++<100)) {
				fScheCallback(ii);
				boost::this_thread::sleep(boost::posix_time::millisec(10));//�ȴ�
			}
			if (is_running) {
				is_running = false;
				m_state = 0;
				return;//�����˳�
			}
			else {
				m_state = 1;
				return;//�������˳���is_running���ⲿ�ı��
			}
			
		}

	};

}