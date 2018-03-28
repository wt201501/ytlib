#pragma once
#include <ytlib/Common/Util.h>
#include <cmath>
#include <vector>

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
		uint32_t tmp = uint32_t(std::sqrt(num));
		//��6�ı��������Ҳ���ܲ�������  
		for (uint32_t i = 5; i <= tmp; i += 6)
			if (num %i == 0 || num % (i + 2) == 0)
				return false;
		//�ų����У�ʣ���������  
		return true;

	}
	//�����Լ��
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
	//����С��������num1*num2/gcd(num1,num2)
	static uint32_t lcm(uint32_t num1, uint32_t num2) {
		return num1 / gcd(num1, num2) * num2;
	}
	//�ֽ�������
	static void factoring(uint32_t num, std::vector<uint32_t>& re) {
		if (num < 2) return;
		for (uint32_t ii = 2; ii <= num; ++ii) {
			while (num%ii == 0) {
				re.push_back(ii);
				num /= ii;
			}
		}
	}
	//�۳ˣ��豣֤n>=m����m!=0
	static uint64_t Mul(uint64_t n, uint64_t m = 1) {
		assert(n >= m && m);
		uint64_t re = n;
		while ((--n) >= m) re *= n;
		return re;
	}

	//���������Ӵ�n����ͬԪ���У���ȡm(m��n,m��n��Ϊ��Ȼ������Ԫ�أ������еĸ�����A(n,m)=n!/(n-m)!
	static uint64_t Arn(uint64_t n, uint64_t m) {
		assert(n >= m && m);
		return Mul(n, n - m + 1);
	}

	//�������C(n,m)=A(n,m)/m!=n!/(m!*(n-m)!)��C(n,m)=C(n,n-m)
	static uint64_t Crn(uint64_t n, uint64_t m) {
		assert(n >= m && m);
		return (n == m) ? 1 : (Mul(n, m + 1) / Mul(n - m));
	}

	//�Ȳ��������
	static tfloat SumAP(tfloat a1, tfloat d, uint64_t n) {
		return n * (a1 + d / 2 * (n - 1));
	}

	//�ȱ��������
	static tfloat SumGP(tfloat a1, tfloat q, uint64_t n) {
		assert(q != 0.0);
		return (q == 1.0) ? (n*a1) : (a1*(1 - std::pow(q, n)) / (1 - q));
	}


}