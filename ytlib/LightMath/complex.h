#pragma once

#include <ytlib/LightMath/mathbase.h>

#include <iostream>
#include <sstream>


namespace ytlib
{
	class  Complex {
	public:

		Complex() :real(0.0), imag(0.0) {}
		Complex(const tfloat a, const tfloat b) : real(a), imag(b) {}
		Complex(const Complex &value) : real(value.real), imag(value.imag) {}
		~Complex() {}

		Complex  operator+ (const Complex &value) {
			return Complex(this->real + value.real, this->imag + value.imag);
		}
		Complex&  operator+= (const Complex &value) {
			this->real += value.real;
			this->imag += value.imag;
			return *this;
		}

		Complex  operator- (const Complex &value) {
			return Complex(this->real - value.real, this->imag - value.imag);
		}
		Complex&  operator-= (const Complex &value) {
			this->real -= value.real;
			this->imag -= value.imag;
			return *this;
		}

		Complex  operator* (const Complex &value) {
			return Complex(
				this->real*value.real - this->imag * value.imag,
				this->real*value.imag + this->imag * value.real
			);
		}
		Complex&  operator*= (const Complex &value) {
			tfloat tmp = this->real*value.real - this->imag * value.imag;
			this->imag = this->real*value.imag + this->imag * value.real;
			this->real = tmp;
			return *this;
		}

		Complex  operator* (const tfloat &s) {
			return Complex(this->real*s, this->imag*s);
		}
		Complex&  operator*= (const tfloat &s) {
			this->real *= s;
			this->imag *= s;
			return *this;
		}

		Complex  operator/ (const Complex &value) {
			tfloat abs_value = Complex::abs(value);
			return Complex(
				(this->real*value.real + this->imag * value.imag) / abs_value,
				(this->real*value.imag - this->imag * value.real) / abs_value
			);
		}
		Complex&  operator/= (const Complex &value) {
			tfloat abs_value = Complex::abs(value);
			tfloat tmp = (this->real*value.real + this->imag * value.imag) / abs_value;
			this->imag = (this->real*value.imag - this->imag * value.real) / abs_value;
			this->real = tmp;
			return *this;
		}

		Complex  operator/ (const tfloat &s) {
			return Complex(this->real / s, this->imag / s);
		}
		Complex&  operator/= (const tfloat &s) {
			this->real /= s;
			this->imag /= s;
			return *this;
		}

		Complex  operator- () {
			return Complex(-(this->real), -(this->imag));
		}

		Complex& swap(Complex &value) {
			if (this != &value) {
				tfloat tmp = this->real;
				this->real = value.real; value.real = tmp;
				tmp = this->imag;
				this->imag = value.imag; value.imag = tmp;
			}
			return *this;
		}

		static Complex conj(const Complex &value) {
			return Complex(value.real, -value.imag);
		}
		static tfloat abs(const Complex &value) {
			return std::sqrt(value.real * value.real + value.imag * value.imag);
		}
		static tfloat angle(const Complex &value) {
			return std::atan2(value.imag, value.real);
		}

		static Complex sqrt(const Complex &value) {
			tfloat a = std::sqrt(abs(value));
			tfloat t = angle(value) / 2;
			return Complex(a*std::cos(t), a*std::sin(t));
		}



		friend std::ostream &operator<<(std::ostream& output, const Complex &rhs) {
			output << rhs.real;
			std::stringstream ss;
			ss << rhs.imag;
			std::string tmp(ss.str());
			if (tmp[0] != '-') {
				output << "+";
			}
			output << tmp << "i";
			return output;
		}

		// direct data access
		tfloat real;
		tfloat imag;
	};

	//��ʵ��������չ�ɸ�������
	static void get_complex(int32_t n, tfloat in[], Complex out[]) {
		for (int32_t i = 0; i < n; ++i) {
			out[i].real = in[i];
			out[i].imag = 0;
		}
	}
	//ȡ����
	static void conjugate_complex(int32_t n, Complex in[]) {
		for (int32_t i = 0; i < n; ++i) {
			in[i].imag = -in[i].imag;
		}
	}
	//��������ȡģ
	static void c_abs(int32_t n, Complex f[], tfloat out[]) {
		for (int32_t i = 0; i < n; ++i) {
			out[i] = Complex::abs(f[i]);
		}
	}
	//����Ҷ�任 ���Ҳ��������f��
	static void fft(int32_t N, Complex f[]) {
		int32_t k, M = 1;
		/*----����ֽ�ļ���M=log2(N)----*/
		for (int32_t i = N; (i /= 2) != 1; ++M);
		/*----���յ�λ����������ԭ�ź�----*/
		for (int32_t i = 1, j = N / 2; i <= N - 2; ++i) {
			if (i < j) {
				f[j].swap(f[i]);
			}
			k = N / 2;
			while (k <= j) {
				j -= k;
				k /= 2;
			}
			j += k;
		}
		/*----FFT�㷨----*/
		int32_t r, la, lb, lc;
		for (int32_t m = 1; m <= M; ++m) {
			la = static_cast<int32_t>(pow(2.0, m)); //la=2^m�����m��ÿ�����������ڵ���
			lb = la / 2;    //lb�����m��ÿ�������������ε�Ԫ��,ͬʱ��Ҳ��ʾÿ�����ε�Ԫ���½ڵ�֮��ľ���
							/*----��������----*/
			for (int32_t l = 1; l <= lb; ++l) {
				r = (l - 1)* static_cast<int32_t>(pow(2.0, M - m));
				//����ÿ�����飬��������ΪN/la
				for (int32_t n = l - 1; n < N - 1; n += la) {
					lc = n + lb;  //n,lc�ֱ����һ�����ε�Ԫ���ϡ��½ڵ���
					Complex t(f[lc] * Complex(cos(2 * PI*r / N), -sin(2 * PI*r / N)));
					f[lc] = f[n] - t;
					f[n] += t;
				}
			}
		}
	}
	// ����Ҷ��任
	static void ifft(int32_t N, Complex f[]) {
		conjugate_complex(N, f);
		fft(N, f);
		conjugate_complex(N, f);
		for (int32_t i = 0; i < N; ++i) {
			f[i] /= N;
		}
	}
	static void fftshift(int32_t len, Complex f[]) {
		len /= 2;
		for (int32_t i = 0; i<len; ++i) {
			f[i + len].swap(f[i]);
		}
	}

	inline tfloat abs(const Complex &value) { return Complex::abs(value); }
	inline Complex sqrt(const Complex &value) { return Complex::sqrt(value); }
}

