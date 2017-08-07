#pragma once

#include <ytlib/SupportTools/QueueBase.h>
#include <boost/thread.hpp>
#include <vector>

namespace ytlib {
	//单队列，异步添加，可以多线程处理数据的通道
	//从进入通道到取出通道数据会经过复制操作。因此建议始终传递share_ptr一类的指针
	//使用阻塞取出，无法暂停，一旦开启将一直取出数据进行处理，直到无数据可取时阻塞
	template<class T,
	class _Queue = QueueBase<T> >
	class ChannelBase {
	public:
		//单处理线程的通道适用于顺序数据处理，多处理线程的通道适用于并行处理
		ChannelBase(std::function<void(T&)> f_, size_t thCount_ = 1, size_t queueSize_ = 1000) :
			m_processFun(f_),
			m_threadCount(thCount_), 
			m_bRunning(true),
			m_queue(queueSize_) {
			for (size_t ii = 0; ii < m_threadCount; ++ii) {
				m_Threads.create_thread(std::bind(&ChannelBase::Run, this));
			}
		}
		virtual ~ChannelBase(void) {
			//结束线程，禁止添加
			m_bRunning = false;
			m_Threads.join_all();
		}
		inline bool Add(const T &item_) {
			return m_queue.Enqueue(item_);
		}
		inline void Clear() {m_queue.Clear();}
		inline double GetUsagePercentage() {return (double)(m_queue.Count()) / (double)(m_queue.GetMaxCount());	}
	private:
		//线程运行函数
		void Run() {
			while (m_bRunning) {
				T item_;
				if (m_queue.BlockDequeue(item_)) 
					m_processFun(item_);//处理数据，此处的处理不支持异步
			}
		}

	protected:
	
		std::atomic_bool m_bRunning;
		boost::thread_group m_Threads;
		_Queue m_queue;
		std::function<void(T&)> m_processFun;//处理内容函数。参数推荐使用（智能）指针
		const size_t m_threadCount;
		
	};

}