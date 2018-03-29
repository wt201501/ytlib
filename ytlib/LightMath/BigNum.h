#pragma once
#include <iostream>
#include <ytlib/LightMath/Mathbase.h>
#include <vector>

//�����������ߡ�todo������
namespace ytlib
{
	class BigNum {
	public:
		explicit BigNum(bool symbol = true, uint32_t _up = 10) :_symbol(symbol), up(_up) {
			assert(checkNum());
		}
		virtual ~BigNum() {}


		explicit BigNum(const std::vector<uint32_t>& data, bool symbol = true, uint32_t _up = 10) :_symbol(symbol), _content(data), up(_up) {
			assert(checkNum());
		}
		BigNum(const uint32_t* a, size_t sz, bool symbol = true, uint32_t _up = 10) :_symbol(symbol), up(_up) {
			_content.reserve(sz);
			_content.assign(a, a + sz);
			assert(checkNum());
		}
		//��������һ��int64��ʽת����BigNum
		BigNum(int64_t num, uint32_t _up = 10) :_symbol(num >= 0), up(_up) {
			_content.push_back(static_cast<uint32_t>(num));
			_content.push_back(static_cast<uint32_t>(num>>32));
			assert(checkNum());
		}


		BigNum  operator+ (const BigNum &value) const {
			if (_content.size() < value._content.size()) return value + (*this);
			size_t ii = 0, len1 = _content.size(), len2 = value._content.size();

			while (ii < len1 && ii < len2) {

			}

		}
		BigNum&  operator+= (const BigNum &value) {
			
			return *this;
		}
		BigNum& operator++() {

			return *this;
		}
		const BigNum operator++(int) {

			
		}


		BigNum  operator- (const BigNum &value) const {
			
		}
		BigNum&  operator-= (const BigNum &value) {
			
			return *this;
		}

		BigNum& operator--() {

			return *this;
		}
		const BigNum operator--(int) {


		}

		BigNum  operator* (const BigNum &value) const {
			
		}
		BigNum&  operator*= (const BigNum &value) {
			
			return *this;
		}
		BigNum  operator/ (const BigNum &value) const {
			
		}
		BigNum&  operator/= (const BigNum &value) {
			
			return *this;
		}

		bool operator==(const BigNum &value) const {

		}
		//�Ƿ�Ϊ0
		operator bool() const {
			return ((_content.size() == 1) && (_content[0] == 0));
		}
		BigNum  operator- () const {
			BigNum re(*this);
			re._symbol = !re._symbol;
			return re;
		}

		//��ʮ������ʽ����
		friend std::istream& operator>>(std::istream& in, const BigNum& M) {

			return in;
		}

		friend std::ostream& operator<< (std::ostream& out, const BigNum& M) {
			//���������

			//����λ
			if (!_symbol && BigNum::operator bool()) out << '-';

			
			if (up <= 16) {
				//���������16֮�������16���Ƶķ���


			}
			else {
				//������Ҫ�ڸ���λ֮��տ�һ����ʮ�����������


			}
			return out;
		}

	protected:
		//���
		bool checkNum() {
			if ((up < 2) || (up == -1)) return false;//���ٶ����ƣ������Ʋ�����2^32-1

			if (_content.empty()) {
				_content.push_back(0);
				return true;
			}
			size_t len = _content.size();
			for (size_t ii = 0; ii<len; ++ii) {
				if (_content[ii] >= up) return  false;
			}
			return true;
		}


		//����
		bool _symbol;//����
		std::vector<uint32_t> _content;//���ô�˴洢��Խ��λ��Խ���棬��������λ��

		uint32_t up;//����
	};
	
	/*
	���ڴ������ߵ�ѭ���������ߡ�����ʵ��n��ѭ��
	ʹ��ʱ��
	LoopTool lt;
	do{

	}while(--lt);
	*/
	class LoopTool :public BigNum {
	public:
		LoopTool(){}
		virtual ~LoopTool() {}



	private:
		//����һЩ������ֻ֧��+��-
		friend std::istream& operator>>(std::istream& in, const LoopTool& M) {return in;}
		friend std::ostream& operator<< (std::ostream& out, const LoopTool& M) {return out;}




		std::vector<uint32_t> up;//����

	};
	
}

