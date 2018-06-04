#pragma once
#include <ytlib/Common/Util.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

/*
stl���кܶ������㷨����ֱ�ӵ��ã�stlԴ������p288��
�˴��Լ�ʵ�ֵ������㷨��ѧϰ�����
*/

//ģ�廯����������㷨
namespace ytlib {

	//�����˱ȽϷ��ŵ���
	class sortObj {
	public:
		sortObj():key(0){}
		virtual ~sortObj() {}

		sortObj(uint32_t k_) :key(k_) {}

		//��ϵ���������
		bool operator <(const sortObj& val) const {return key < val.key;}
		bool operator >(const sortObj& val) const {return key > val.key;}
		bool operator <=(const sortObj& val) const {return key <= val.key;}
		bool operator >=(const sortObj& val) const {return key >= val.key;}
		bool operator ==(const sortObj& val) const {return key == val.key;}


		//��Ա
		uint32_t key;
		//��Ա����ϴ�Ļ���ָ��ָ��

	};

	//ð������ in-place/�ȶ�
	template<typename T>
	void bubbleSort(T* arr, size_t len) {
		if (len < 2) return;
		using std::swap;
		for (size_t ii = 0; ii < len; ++ii) {
			for (size_t jj = 0; jj < len - 1 - ii; ++jj) {
				if (arr[jj] > arr[jj + 1]) {
					swap(arr[jj], arr[jj + 1]);
				}
			}
		}
	}

	//�鲢���� out-place/�ȶ� todo��ʵ�ַǵݹ���ʽ
	template<typename T>
	void mergeSort(T* arr, size_t len) {
		if (len < 2) return;
		if (len == 2) {
			using std::swap;
			if (arr[0] > arr[1]) swap(arr[0], arr[1]);
			return;
		}
		size_t middle = len / 2;
		mergeSort(arr, middle);
		mergeSort(arr + middle, len - middle);
		//������Ŀռ���������ٸ��ƻ���
		T* tmpArr = new T[len];
		size_t lc = 0, rc = middle, tc = 0;
		while ((lc<middle) && (rc<len)) {
			tmpArr[tc++] = (arr[lc] < arr[rc]) ? arr[lc++] : arr[rc++];
		}
		if (lc < middle && rc == len) memcpy(arr + len - middle + lc, arr + lc, (middle - lc) * sizeof(T));
		memcpy(arr, tmpArr, tc * sizeof(T));
		delete[] tmpArr;
	}

	//�������� in-place/���ȶ� todo��ʵ�ַǵݹ���ʽ
	template<typename T>
	void quickSort(T* arr, size_t len) {
		if (len < 2) return;
		using std::swap;
		if (len == 2) {
			if (arr[0] > arr[1]) swap(arr[0], arr[1]);
			return;
		}
		size_t first = 0, last = len - 1, cur = first;
		while (first < last) {
			while (first < last && arr[last] >= arr[cur]) --last;
			if (cur != last) {
				swap(arr[cur], arr[last]);
				cur = last;
			}
			while (first < last && arr[first] <= arr[cur]) ++first;
			if (cur != first) {
				swap(arr[cur], arr[first]);
				cur = first;
			}
		}
		if (cur > 1) quickSort(arr, cur);
		if (cur < len - 2) quickSort(arr + cur + 1, len - cur - 1);
	}


	//���ֲ��ҡ�Ӧ��������õ�������
	template<typename T>
	size_t binarySearch(T* arr, size_t len,const T& key) {
		assert(len);
		size_t low = 0, high = len - 1;
		while (low <= high) {
			size_t mid = low + (high - low) / 2;
			if (arr[mid] < key) low = mid + 1;
			else if (arr[mid] > key) high = mid - 1;
			else return mid;
		}
		//û�ҵ�������len
		return len;
	}

}


