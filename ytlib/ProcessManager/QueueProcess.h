#pragma once
#include <ytlib/ProcessManager/ProcessBase.h>
#include <ytlib/SupportTools/QueueBase.h>

namespace ytlib
{
	//��ˮ����Process
	
	//��ͣ��ֹֻͣ����ͣ������ݣ��������ݵ��̲߳���Ӱ��
	//�ȵ������е����ݴ���������Ȼ����ͣ������

	//�����̰߳�ȫ����Ҫ��������

	//��ͨ�������������Ǵ˴��Ĵ��������������صģ��˴�����Ӳ�������ͣ����
	//���ǻ��Ǹо�ͨ���ȽϺ��á��������п���û��ʲô��

	template<class T,
	class _Queue = QueueBase<T>>
	class QueueProcess : public ProcessBase{
	public:
		
		QueueProcess(size_t thCount_ = 1, size_t queueSize_ = 1000) :
			ProcessBase() ,
			m_bStopFlag(false),
			m_threadCount(thCount_),
			m_queue(queueSize_)	{	
			
		}
		virtual ~QueueProcess() {
			stop();
		}
		virtual bool init() {
			if (!m_threadVec.empty()) return false;
			m_bStopFlag = false;
			for (size_t ii = 0; ii < m_threadCount; ii++) {
				m_threadVec.push_back(new std::thread(&QueueProcess::Run, this));
			}
			return ProcessBase::init();
		}

		//��������ʱһ��Ҫ������
		virtual bool stop() {
			if (m_bStopFlag || !ProcessBase::stop()) return false;
			//stop֮����Ҫ���³�ʼ��
			is_init = false;
			m_bStopFlag = true;
			for (size_t ii = 0; ii < m_threadCount; ii++) {
				m_threadVec[ii]->join();//�ȴ������߳̽���
				delete m_threadVec[ii];
			}
			m_threadVec.clear();
			return true;
		}

		//start֮�����add
		virtual bool Add(const T& item_) {
			return is_running && m_queue.Enqueue(item_);
		}
	private:
		//�߳����к���
		void Run() {
			while (!m_bStopFlag) {
				T item_;
				if (m_queue.BlockDequeue(item_))
					ProcFun(item_);//�������ݣ��˴��Ĵ���֧���첽
			}
		}
	protected:
		
		//����������������
		virtual void ProcFun(const T&) = 0;

		std::atomic_bool m_bStopFlag;
		std::vector<std::thread*> m_threadVec;
		_Queue m_queue;
		const size_t m_threadCount;
	};
}