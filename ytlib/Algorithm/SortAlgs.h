#pragma once
#include <ytlib/Common/Util.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

/*
stl���кܶ������㷨����ֱ�ӵ��ã�stlԴ������p288��
�˴��Լ�ʵ�ֵ������㷨��ѧϰ�����
todo��������
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
		bool operator <(const sortObj& val) {return key < val.key;}
		bool operator >(const sortObj& val) {return key > val.key;}
		bool operator <=(const sortObj& val) {return key <= val.key;}
		bool operator >=(const sortObj& val) {return key >= val.key;}
		bool operator ==(const sortObj& val) {return key == val.key;}


		//��Ա
		uint32_t key;
		//��Ա����ϴ�Ļ���ָ��ָ��

	};



	//ð������ in-place/�ȶ�
	template<typename T>
	void bubbleSort(T* arr, size_t len) {
		using std::swap;
		for (size_t ii = 0; ii < len; ++ii) {
			for (size_t jj = 0; jj < len - 1 - ii; ++jj) {
				if (arr[jj] > arr[jj + 1]) {
					swap(arr[jj], arr[jj + 1]);
				}
			}
		}
	}



	//�鲢���� out-place/�ȶ���todo����û���
	template<typename T>
	void mergeSort(T* arr, size_t len) {
		if (len < 2) return;
		size_t llen = len / 2, rlen = len - llen;
		
		mergeSort(arr, llen);
		mergeSort(arr + llen, rlen);
		//������Ŀռ���������ٸ��ƻ���
		T* tmpArr = new T[len];
		size_t lc = 0, rc = llen, tc = 0;
		while ((lc<llen) && (rc<len)) {
			if (arr[lc] < arr[rc]) {
				tmpArr[tc++] = arr[lc++];
			}
			else {
				tmpArr[tc++] = arr[rc++];
			}
		}

		if (lc < llen && rc==len) {
			memcpy(arr + lc + llen, arr + lc, (llen - lc) * sizeof(T));
		}
		memcpy(arr, tmpArr, tc * sizeof(T));
		delete[] tmpArr;
	}

	//�������� in-place/���ȶ�
	template<typename T>
	void quickSort(T* arr, size_t len) {
		using std::swap;


	}



	//
	


	//���ֲ���



}


