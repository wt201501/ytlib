#pragma once
#include <ytlib/Common/Util.h>
#include <cmath>

#define USE_DOUBLE_PRECISION

#if defined(USE_DOUBLE_PRECISION)
typedef double tfloat;      // double precision
#else 
typedef float  tfloat;    // single precision
#endif 

#define PI 3.1415926535897932384626433832795028841971

//�жϸ������Ƿ����
#ifndef fequal
#define fequal(a,b)          (((a-b)>-1e-6)&&((a-b)<1e-6))
#endif

//һЩ����
namespace ytlib
{
	//�����ж��ٸ�1
	static uint32_t count_1_(uint32_t n) {
		uint32_t num = 0;
		while (n) {
			n &= (n - 1);
			++num;
		}
		return num;
	}

	//�����ж��ٸ�0
	static uint32_t count_0_(uint32_t n) {
		return count_1_(~n);
	}

	//�ж��Ƿ�������
	static bool isPrime(uint32_t num) {
		//������С�����⴦��  
		if (num == 2 || num == 3)
			return true;
		//����6�ı��������һ����������  
		if (num % 6 != 1 && num % 6 != 5)
			return false;
		int tmp = std::sqrt(num);
		//��6�ı��������Ҳ���ܲ�������  
		for (int i = 5; i <= tmp; i += 6)
			if (num %i == 0 || num % (i + 2) == 0)
				return false;
		//�ų����У�ʣ���������  
		return true;

	}
	//�����Լ������С��������num1*num2/gcd(num1,num2)
	static uint32_t gcd(uint32_t num1, uint32_t num2) {
		if (num1 < num2) return gcd(num2, num1);
		if (num2 == 0) return num1;
		if (num1 & 1) {
			if (num2 & 1) return gcd(num2, num1 - num2);
			return gcd(num1, num2 >> 1);

		}
		else {
			if (num2 & 1) return gcd(num1 >> 1, num2);
			return (gcd(num1 >> 1, num2 >> 1) << 1);
		}
	}




}