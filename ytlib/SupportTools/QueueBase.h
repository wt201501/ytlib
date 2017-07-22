#pragma once

#include <queue>
#include <atomic>
#include <mutex>

namespace ytlib {

	//�򵥵Ķ��̻߳������̰߳�ȫ�Ķ���
	template <class T>
	class QueueBase{
	public:
		explicit QueueBase(size_t n_):m_maxcount(n_){}
		virtual ~QueueBase(){}
		inline size_t GetMaxCount(){ return m_maxcount; }
		inline size_t Count() {
			std::lock_guard<std::mutex> lck(m_mutex);
			return m_queue.size();
		}
		inline void Clear() {
			std::lock_guard<std::mutex> lck(m_mutex);
			m_queue.swap(std::queue<T>());
		}
		//���Ԫ��
		bool Enqueue(const T &item) {
			std::lock_guard<std::mutex> lck(m_mutex);
			if (m_queue.size() < m_maxcount) {
				m_queue.push(std::move(item));
				m_cond.notify_one();
				return true;
			}
			return false;
		}
		//ȡ��Ԫ��
		bool Dequeue(T &item) {
			std::lock_guard<std::mutex> lck(m_mutex);
			if (m_queue.empty()) return false; 
			item = std::move(m_queue.front());
			m_queue.pop();
			return true;

		}
		
		//����ʽȡ����������˾�һֱ�ȴ�������ȡ������û��������ʽ��ӣ���Ϊһ�㲻�����õ���
		bool BlockDequeue(T &item) {
			std::unique_lock<std::mutex> lck(m_mutex);
			m_cond.wait(lck, [this] {return !(this->m_queue.empty()); });
			item = std::move(m_queue.front());
			m_queue.pop();
			return true;
		}

	protected:
		std::mutex m_mutex;//ͬ�������˴������ö�д������Ϊcondition_variable����֧��
		std::condition_variable m_cond;//������
		std::queue<T> m_queue;// ����
		
		const size_t m_maxcount;//���п�֧��������
	};
}