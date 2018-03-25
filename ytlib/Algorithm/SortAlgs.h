#pragma once
#include <ytlib/Common/Util.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

/*
stl���кܶ������㷨����ֱ�ӵ��ã�stlԴ������p288��
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


	};


	

}


