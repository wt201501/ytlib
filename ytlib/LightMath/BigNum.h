#pragma once
#include <iostream>
#include <ytlib/LightMath/Mathbase.h>
#include <vector>

//���������ߡ�todo������
namespace ytlib
{
	class BigNum {
	public:
		explicit BigNum(int64_t num = 0, uint32_t _up = 0) :_symbol(num >= 0), up(0) {
			assert(_up != 1);
			num = std::abs(num);
			_content.push_back(static_cast<uint32_t>(num));
			if (num >>= 32)	_content.push_back(static_cast<uint32_t>(num));
			changeNumSys(_up);
		}

		BigNum  operator+ (const BigNum &value) const {
			//��Ҫȷ��������ͬ
			assert(up == value.up);
			const BigNum* pNum1 = this, *pNum2 = &value;
			size_t len1 = pNum1->_content.size(), len2 = pNum2->_content.size();
			BigNum re(0, up);
			if (_symbol^value._symbol) {
				//�������ӣ��þ���ֵ��ļ�С�ģ�����������ͬ��Ĭ��num1�ľ���ֵ��
				if (len1 == len2) {
					//�Ӹ�λ��ʼ�ж�
					for (size_t ii = len1 - 1; ii > 0; --ii) {
						if (pNum1->_content[ii] > pNum2->_content[ii]) break;
						else if (pNum1->_content[ii] < pNum2->_content[ii]) {
							std::swap(len1, len2);
							std::swap(pNum1, pNum2);
							break;
						}
					}
				}
				else if (len1 < len2) {
					std::swap(len1, len2);
					std::swap(pNum1, pNum2);
				}
				re._symbol = pNum1->_symbol;
				bool flag = false;//��λ��־
				for (size_t ii = 0; ii < len2; ++ii) {
					//��Ҫ��λ�����
					if (flag && pNum1->_content[ii] == 0) {
						flag = true;
						re._content[ii] = up - 1 - pNum2->_content[ii];
					}
					else if ((pNum1->_content[ii]-(flag?1:0)) < pNum2->_content[ii]) {
						flag = true;
						re._content[ii] = up - pNum2->_content[ii] + (pNum1->_content[ii] - (flag ? 1 : 0));
					}
					else {
						flag = false;
						re._content[ii] = pNum1->_content[ii] - pNum2->_content[ii];
					}
					re._content.push_back(0);
				}
				for (size_t ii = len2; ii < len1; ++ii) {
					if (flag && pNum1->_content[ii] == 0) {
						flag = true;
						re._content[ii] = up - 1;
					}
					else {
						flag = false;
						re._content[ii] = pNum1->_content[ii];
					}
					re._content.push_back(0);
				}
			}
			else {
				//ͬ�������
				re._symbol = _symbol;
				//������num1��λ���ϴ�
				if (len1 < len2) {
					std::swap(len1, len2);
					std::swap(pNum1, pNum2);
				}
				//�ӵ�λ��ʼ��
				for (size_t ii = 0; ii < len2; ++ii) {
					uint32_t tmp = pNum1->_content[ii] + pNum2->_content[ii] + re._content[ii];
					if ((up && tmp >= up) || tmp < pNum1->_content[ii] || (tmp == pNum1->_content[ii] && re._content[ii] == 1)) {
						re._content.push_back(1);
						tmp -= up;
					}
					else re._content.push_back(0);
					re._content[ii] = tmp;
				}
				for (size_t ii = len2; ii < len1; ++ii) {
					if (pNum1->_content[ii]==(up-1) && re._content[ii] == 1) {
						re._content[ii] = 0;
						re._content.push_back(1);
					}
					else {
						re._content[ii] += pNum1->_content[ii];
						re._content.push_back(0);
					}
				}
			}
			//ȥ�����˵�0
			while (re._content.size() > 1 && re._content[re._content.size() - 1] == 0) re._content.pop_back();
			return std::move(re);
		}
		BigNum&  operator+= (const BigNum &value) {
			(*this) = operator+(value);
			return *this;
		}
		//++i
		BigNum& operator++() {
			operator+=(BigNum(1, up));
			return *this;
		}
		//i++
		const BigNum operator++(int) {
			BigNum re(*this);
			operator+=(BigNum(1, up));
			return re;
		}

		BigNum  operator- (const BigNum &value) const {
			return operator+(-value);
		}
		BigNum&  operator-= (const BigNum &value) {
			(*this) = operator+(-value);
			return *this;
		}

		BigNum& operator--() {
			operator+=(BigNum(-1, up));
			return *this;
		}
		const BigNum operator--(int) {
			BigNum re(*this);
			operator+=(BigNum(-1, up));
			return re;
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
			return (*this);
		}
		BigNum  operator% (const BigNum &value) const {
			assert(up == value.up);

		}
		BigNum&  operator%= (const BigNum &value) {
			assert(up == value.up);
			return (*this);
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
				if (_content[ii] == value._content[ii]) continue;
				if (_symbol ^ (_content[ii] > value._content[ii])) return true;
				return false;
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
		friend std::istream& operator>>(std::istream& in, BigNum& val) {
			val._content.clear();
			val.up = 10;


			return in;
		}

		friend std::ostream& operator<< (std::ostream& out, const BigNum& val) {
			//���������

			//����λ
			if (!val._symbol && !val) out << '-';

			
			if (val.up <= 16) {
				//���������16֮�������16���Ƶķ���


			}
			else {
				//������Ҫ�ڸ���λ֮��տ�һ����ʮ�����������


			}
			return out;
		}
		static BigNum abs(const BigNum& val) {
			BigNum re(val);
			re._symbol = true;
			return std::move(re);
		}
		//����ͬʱ���ؽ��������
		std::pair<BigNum, BigNum> div(const BigNum& val) const {
			//����ֻ��һλ


		}

		//�ı����
		void changeNumSys(uint32_t up_) {
			assert(up_ != 1);
			if (up_ == up) return;
			std::vector<uint32_t> tmp;
		



			up = up_;
			_content = std::move(tmp);
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
	...
	}while(--lt);
	*/
	class LoopTool {
	public:
		explicit LoopTool(const std::vector<uint32_t>& up_):up(up_){
			size_t len = up.size();
			assert(len);
			_content.resize(len);
			for (size_t ii = 0; ii < len; ++ii) {
				_content[ii] = 0;
			}
		}

		//���ֻ�ܼӵ�up-1
		//++i
		LoopTool& operator++() {
			size_t len = up.size();
			assert(len);
			//�ӵ�λ��ʼ��
			for (size_t ii = 0; ii < len; ++ii) {
				_content[ii] += 1;
				if (_content[ii] == up[ii]) _content[0] = 0;
				else break;
			}
			return *this;
		}
		LoopTool& operator--() {
			size_t len = up.size();
			assert(len);
			//�ӵ�λ��ʼ��
			for (size_t ii = 0; ii < len; ++ii) {
				if (_content[ii] == 0) _content[0] = up[ii] - 1;
				else {
					_content[ii] -= 1;
					break;
				}
			}
			return *this;
		}
		//�Ƿ�Ϊ0��0Ϊfalse
		operator bool() const {
			size_t len = up.size();
			for (size_t ii = 0; ii < len; ++ii) {
				if (_content[ii]) return true;
			}
			return false;
		}

		std::vector<uint32_t> _content;
		std::vector<uint32_t> up;//����

	};
	
}

