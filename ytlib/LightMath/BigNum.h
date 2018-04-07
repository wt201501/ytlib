#pragma once
#include <iostream>
#include <ytlib/LightMath/Mathbase.h>
#include <vector>

//���������ߡ�todo������
namespace ytlib
{
	class BigNum {
	public:
		explicit BigNum(bool symbol = true, uint32_t _up = 10) :_symbol(symbol), up(_up) {
			assert(up != 1);
			_content.push_back(0);
		}
		//��������һ��int64��ʽת����BigNum
		BigNum(int64_t num, uint32_t _up = 0) :_symbol(num >= 0), up(0) {
			num = std::abs(num);
			_content.push_back(static_cast<uint32_t>(num));
			_content.push_back(static_cast<uint32_t>(num>>32));
			changeNumSys(_up);
		}

		BigNum  operator+ (const BigNum &value) const {
			//��Ҫȷ��������ͬ
			assert(up == value.up);
			//��������λ���ϴ�
			if (_content.size() < value._content.size()) return value + (*this);
			size_t len1 = value._content.size(), len2 = _content.size();
			BigNum re(true, up);
			if (_symbol^value._symbol) {
				//�������ӣ��ô�ļ�С��

			}
			else {
				//ͬ�������
				re._symbol = _symbol;
				//�ӵ�λ��ʼ��
				for (size_t ii = 0; ii < len1; ++ii) {
					uint32_t tmp = _content[ii] + value._content[ii] + re._content[ii];
					if (tmp >= up || tmp < _content[ii] || (tmp == _content[ii] && re._content[ii] == 1)) {
						re._content.push_back(1);
						tmp -= up;
					}
					else re._content.push_back(0);
					re._content[ii] = tmp;
				}
				for (size_t ii = len1; ii < len2; ++ii) {
					if ( _content[ii]==(up-1) && re._content[ii] == 1) {
						re._content[ii] = 0;
						re._content.push_back(1);
					}
					else {
						re._content[ii] += _content[ii];
						re._content.push_back(0);
					}
				}
			}
			//ȥ�����˵�0
			if (re._content.size() > 1 && re._content[re._content.size() - 1] == 0) re._content.pop_back();

		}
		BigNum&  operator+= (const BigNum &value) {
			assert(up == value.up);


			return *this;
		}
		//++i
		BigNum& operator++() {

			return *this;
		}
		//i++
		const BigNum operator++(int) {

			
		}


		BigNum  operator- (const BigNum &value) const {
			assert(up == value.up);

		}
		BigNum&  operator-= (const BigNum &value) {
			assert(up == value.up);

			return *this;
		}

		BigNum& operator--() {

			return *this;
		}
		const BigNum operator--(int) {


		}

		BigNum  operator* (const BigNum &value) const {
			assert(up == value.up);

		}
		BigNum&  operator*= (const BigNum &value) {
			assert(up == value.up);

			return *this;
		}
		BigNum  operator/ (const BigNum &value) const {
			assert(up == value.up);

		}
		BigNum&  operator/= (const BigNum &value) {
			assert(up == value.up);

			return *this;
		}

		bool operator==(const BigNum &value) const {
			assert(up == value.up);
			//�������0
			if (BigNum::operator bool() && value) return true;
			if (_symbol != value._symbol) return false;
			if (_content.size() != value._content.size()) return false;
			size_t len = _content.size();
			for (size_t ii = 0; ii < len; ++ii) {
				if (_content[ii] != value._content[ii]) return false;
			}
			return true;
		}
		//�Ƿ�Ϊ0��0Ϊfalse
		operator bool() const {
			return !((_content.size() == 1) && (_content[0] == 0));
		}
		BigNum  operator- () const {
			BigNum re(*this);
			re._symbol = !re._symbol;
			return re;
		}
		bool operator <(const BigNum& value) const {
			assert(up == value.up);
			if (BigNum::operator bool() && value) return false;
			if (!_symbol && value._symbol) return true;
			if (_symbol && !(value._symbol)) return false;
			if (_content.size() != value._content.size()) return _symbol ^ (_content.size() > value._content.size());
			size_t len = _content.size();
			//�Ӹ�λ��ʼ�ж�
			for (size_t ii = len - 1; ii > 0; --ii) {
				if (_symbol ^ (_content[ii] >= value._content[ii])) return true;
			}
			return _symbol ^ (_content[0] >= value._content[0]);
		}
		bool operator >(const BigNum& value) const {
			return value < (*this);
		}
		bool operator <=(const BigNum& value) const {
			return BigNum::operator<(value) || BigNum::operator==(value);
		}
		bool operator >=(const BigNum& value) const {
			return BigNum::operator>(value) || BigNum::operator==(value);
		}

		//��ʮ������ʽ����
		friend std::istream& operator>>(std::istream& in, const BigNum& M) {

			return in;
		}

		friend std::ostream& operator<< (std::ostream& out, const BigNum& M) {
			//���������

			//����λ
			if (!_symbol && !(BigNum::operator bool())) out << '-';

			
			if (up <= 16) {
				//���������16֮�������16���Ƶķ���


			}
			else {
				//������Ҫ�ڸ���λ֮��տ�һ����ʮ�����������


			}
			return out;
		}

		//�ı����
		void changeNumSys(uint32_t up_) {
			assert(up_ != 1);
			if (up_ == up) return;


		}

	protected:

		bool _symbol;//����
		std::vector<uint32_t> _content;//���ô�˴洢��Խ��λ��Խ���棬��������λ��
		uint32_t up;//���ƣ����ܵ���1��0��ʾ������2^32Ϊ����
	};
	
	/*
	���ڴ������ߵ�ѭ���������ߡ�����ʵ��n��ѭ������Ȼһ�㲻Ҫ����n��ѭ��
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

