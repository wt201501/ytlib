#pragma once
#include <ytlib/Common/Util.h>
#include <cmath>
#include <vector>
#include <map>

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
	static uint32_t count_1_(uint64_t n) {
		uint32_t num = 0;
		while (n) {
			n &= (n - 1);
			++num;
		}
		return num;
	}
	//�����ж��ٸ�0
	static uint32_t count_0_(uint64_t n) {
		return count_1_(~n);
	}

	//������pos֮ǰ�ĵ�һ��1��λ��
	static uint32_t find_1_(uint64_t val, uint32_t pos) {
		assert(pos < 64);
		if (!val) return 64;
		while (0 == (val & (1 << pos))) {
			++pos;
		}
		return pos;
	}

	//�ж��Ƿ�������
	static bool isPrime(uint64_t num) {
		//������С�����⴦��  
		if (num == 2 || num == 3) return true;
		//����6�ı��������һ����������  
		if (num % 6 != 1 && num % 6 != 5) return false;
		uint64_t tmp = uint64_t(std::sqrt(num));
		//��6�ı��������Ҳ���ܲ�������  
		for (uint64_t i = 5; i <= tmp; i += 6)
			if (num %i == 0 || num % (i + 2) == 0)
				return false;
		//�ų����У�ʣ���������  
		return true;

	}
	//�����Լ��
	static uint64_t gcd(uint64_t num1, uint64_t num2) {
		if (num1 < num2) std::swap(num1, num2);
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
	static uint64_t lcm(uint64_t num1, uint64_t num2) {
		return num1 / gcd(num1, num2) * num2;
	}
	//�ֽ�������
	static void factoring(uint64_t num, std::vector<uint64_t>& re) {
		if (num < 2) return;
		for (uint64_t ii = 2; num > 1; ++ii) {
			while (num%ii == 0) {
				re.push_back(ii);
				num /= ii;
			}
		}
	}
	static void factoring(uint64_t num, std::map<uint64_t, uint64_t>& re) {
		if (num < 2) return;
		for (uint64_t ii = 2;  num > 1; ++ii) {
			while (num%ii == 0) {
				std::map<uint64_t, uint64_t>::iterator itr = re.find(ii);
				if (itr == re.end()) re.insert(std::pair<uint64_t, uint64_t>(ii, 1));
				else ++(itr->second);
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
	//�������ݹ���⣬��֪A(n,m_)��A(n,m)
	static uint64_t Arn(uint64_t n, uint64_t m, uint64_t A, uint64_t m_) {
		assert(n >= m && m && n >= m_ && m_);
		if (m < m_) return A / Mul(n - m + 2, n - m_ + 1);
		else if (m > m_) return A * Mul(n - m_ + 2, n - m + 1);
		else return A;
	}

	//�������C(n,m)=A(n,m)/m!=n!/(m!*(n-m)!)��C(n,m)=C(n,n-m)
	static uint64_t Crn(uint64_t n, uint64_t m) {
		assert(n >= m && m);
		if (n >= (2 * m + 1)) m = n - m;
		return (n == m) ? 1 : (Mul(n, m + 1) / Mul(n - m));
	}
	//������ݹ����
	static uint64_t Crn(uint64_t n, uint64_t m, uint64_t C, uint64_t m_) {
		assert(n >= m && m && n >= m_ && m_);
		if (n >= (2 * m + 1)) m = n - m;
		if (m < m_) return C*Mul(m_, m + 1) / Mul(n - m, n - m_ + 1);
		else if (m > m_) return C*Mul(n - m_, n - m + 1) / Mul(m, m_ + 1);
		else return C;
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

	// pow
	static uint64_t pow(uint64_t value, uint64_t n) {
		uint64_t re = 1;
		for (; n; n >>= 1) {
			if (n & 1)
				re *= value;
			value *= value;
		}
		return re;
	}

}