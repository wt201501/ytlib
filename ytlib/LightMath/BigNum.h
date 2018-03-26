#pragma once
#include <iostream>
#include <ytlib/LightMath/mathbase.h>
#include <vector>

//������������
namespace ytlib
{
	class BigNum {
	public:
		BigNum(bool symbol = true) :_symbol(symbol) {}
		virtual ~BigNum(){}

		BigNum(const std::vector<uint32_t>& data, bool symbol = true) :_symbol(symbol), _content(data) {

		}
		BigNum(const uint32_t* a, size_t sz, bool symbol = true) :_symbol(symbol) {
			_content.reserve(sz);
			_content.assign(a, a + sz);
		}

		BigNum  operator+ (const BigNum &value) {
			
		}
		BigNum&  operator+= (const BigNum &value) {
			
			return *this;
		}
		BigNum& operator++() {

		}



		BigNum  operator- (const BigNum &value) {
			
		}
		BigNum&  operator-= (const BigNum &value) {
			
			return *this;
		}

		BigNum  operator* (const BigNum &value) {
			
		}
		BigNum&  operator*= (const BigNum &value) {
			
			return *this;
		}
		BigNum  operator/ (const BigNum &value) {
			
		}
		BigNum&  operator/= (const BigNum &value) {
			
			return *this;
		}



	protected:
		//����
		bool _symbol;//����
		std::vector<uint32_t> _content;
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



	};
	
}

