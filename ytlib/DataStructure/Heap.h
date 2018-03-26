#pragma once
#include <ytlib/Common/Util.h>
#include <vector>

namespace ytlib {
	/*
	stl���й���heap�ĺ�����
	make_heap/pop_heap/push_heap/sort_heap
	*/
#define LEFT_CHILD(x)	((x << 1) + 1)
#define RIGHT_CHILD(x)	((x << 1) + 2)
#define PARENT(x)		((x - 1) >> 1)


	//�ѡ�type=trueΪ��С�ѣ���������
	//T��Ҫ֧�ֱȽ�����
	template<typename T>
	class Heap {
	public:
		Heap(bool _type = true) : type(_type) {}
		virtual ~Heap() {}

		Heap(const std::vector<T>& a, bool _type = true) :container(a), type(_type) {
			adjust();
		}
		Heap(const T* a, size_t sz, bool _type = true) :type(_type) {
			container.reserve(sz);
			container.assign(a, a + sz);
			adjust();
		}

		void assign(const T* a, size_t sz) {
			container.clear();
			container.reserve(sz);
			container.assign(a, a + sz);
			adjust();
		}

		//ѹ��
		void push(const T& val) {
			container.push_back(val);
			adjustUp(container.size() - 1);
		}

		//�����Ѷ�
		void pop() {
			using std::swap;
			assert(!container.empty());
			swap(container[0], container[container.size() - 1]);
			container.pop_back();
			adjustDown(0);
		}

		//����Ϊ��
		void adjust() {
			if (container.empty()) return;
			for (size_t ii = ((container.size() - 2) >> 1); ii > 0; --ii) {
				adjustDown(ii);
			}
			adjustDown(0);
		}

		void adjustDown(size_t index, size_t len = 0) {
			using std::swap;
			size_t &parent = index;
			size_t child = LEFT_CHILD(parent);
			if (len == 0) len = container.size();
			while (child < len) {
				//ѡȡ�����ӽڵ��д�/С��
				if (((child + 1) < len) && ((container[child + 1] > container[child]) ^ type)) {
					++child;
				}
				//����ӽڵ��/С�ڸ��ڵ�
				if ((container[child] > container[parent]) ^ type) {
					swap(container[child], container[parent]);
					parent = child;
					child = LEFT_CHILD(parent);
				}
				else break;
			}

		}

		void adjustUp(size_t index) {
			using std::swap;
			size_t &child = index;
			size_t parent = PARENT(child);
			while (child > 0) {
				//����ӽڵ��/С�ڸ��ڵ�
				if ((container[child]>container[parent])^ type) {
					swap(container[child], container[parent]);
					child = parent;
					parent = PARENT(child);
				}
				else break;
			}
		}
		//��������С�ѵ����ţ����������ţ�������֮������ͷ�ת
		void sort() {
			using std::swap;
			for (size_t ii = container.size() - 1; ii > 0; --ii) {
				swap(container[0], container[ii]);
				adjustDown(0, ii);
			}
			type = !type; //���ͷ�ת
		}

		bool type;
		std::vector<T> container;//����ֱ�ӹ�������
		
	};


}


