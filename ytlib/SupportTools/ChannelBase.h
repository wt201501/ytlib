#pragma once

#include <ytlib/SupportTools/QueueBase.h>
#include <vector>

namespace ytlib {
	//�����У��첽��ӣ����Զ��̴߳������ݵ�ͨ��
	//�ӽ���ͨ����ȡ��ͨ�����ݻᾭ�����Ʋ�������˽���ʼ�մ���share_ptrһ���ָ��
	//ʹ������ȡ�����޷���ͣ��һ��������һֱȡ�����ݽ��д���ֱ�������ݿ�ȡʱ����
	template<class T,
	class _Queue = QueueBase<T>>
	class ChannelBase {
	public:
		//�������̵߳�ͨ��������˳�����ݴ����ദ���̵߳�ͨ�������ڲ��д���
		ChannelBase(std::function<void(const T&)> f_, size_t thCount_ = 1, size_t queueSize_ = 1000) :
			m_processFun(f_),
			m_threadCount(thCount_), 
			m_bRunning(true),
			m_queue(queueSize_) {
			for (size_t ii = 0; ii < m_threadCount; ii++) {
				m_Threads.create_thread(std::bind(&ChannelBase::Run, this));
			}
		}
		virtual ~ChannelBase(void) {
			//�����̣߳���ֹ���
			m_bRunning = false;
			m_Threads.join_all();
		}
		inline bool Add(const T &item_) {
			return m_queue.Enqueue(item_);
		}
		inline void Clear() {m_queue.Clear();}
		inline double GetUsagePercentage() {return (double)(m_queue.Count()) / (double)(m_queue.GetMaxCount());	}
	private:
		//�߳����к���
		void Run() {
			while (m_bRunning) {
				T item_;
				if (m_queue.BlockDequeue(item_)) 
					m_processFun(item_);//�������ݣ��˴��Ĵ���֧���첽
			}
		}

	protected:
	
		std::atomic_bool m_bRunning;
		boost::thread_group m_Threads;
		_Queue m_queue;
		std::function<void(const T&)> m_processFun;//�������ݺ����������Ƽ�ʹ�ã����ܣ�ָ��
		const size_t m_threadCount;
		
	};

}