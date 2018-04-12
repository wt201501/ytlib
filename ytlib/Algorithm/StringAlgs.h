#pragma once
#include <ytlib/Common/Util.h>
#include <string>
#include <map>


//�ַ�������㷨���˴����㷨��ֻ�Ǹ���һ�ֿ��з����������������е���ѷ���
namespace ytlib {

	//�ַ���ƥ��kms�㷨�����û��ƥ�䵽�򷵻�sslen
	static size_t KMP(const char* ss, size_t sslen, const char* ps, size_t pslen) {
		assert(sslen && pslen && ss!=NULL && ps!= NULL);
		if (pslen > sslen || pslen == 0) return sslen;
		int32_t *next = new int32_t[pslen];//����next����
		next[0] = -1;
		int32_t ii = 0, jj = -1;
		--pslen;
		while (ii < pslen) {
			if (jj == -1 || ps[ii] == ps[jj]) next[ii] = (ps[++ii] == ps[++jj]) ? next[jj] : jj;
			else jj = next[jj];
		}
		++pslen;
		ii = jj = 0;
		while (ii < sslen && jj < int32_t(pslen)) {
			if (jj == -1 || ss[ii] == ps[jj]) { ++ii; ++jj; }
			else jj = next[jj];
		}
		delete[] next;
		return (jj == pslen) ? (ii - jj) : sslen;
	}
	static size_t KMP(const std::string& ss, const std::string& ps) {
		return KMP(ss.c_str(), ss.length(), ps.c_str(), ps.length());
	}

	//�����ַ�������ȡ��Ż�����ֻʹ��2*min(s1len,s2len)���ڴ������s1len*s2len���ڴ�
	static size_t StrDif(const char* s1, size_t s1len, const char* s2, size_t s2len) {
		assert(s1len && s2len && s1 != NULL && s2 != NULL);
		if (s2len > s1len) return StrDif(s2, s2len, s1, s1len);//Ĭ��s1len>=s2len

		const uint32_t c1 = 1, c2 = 1;//c1: unmatched cost; c2: mismatched cost
		size_t *M1 = new size_t[s2len], *M2 = new size_t[s2len], *tmpM;
		for (size_t ii = 0; ii < s2len; ++ii) M1[ii] = ii*c1;
		for (size_t ii = 1; ii < s1len; ++ii) {
			for (size_t jj = 0; jj < s2len; ++jj) {
				if (jj == 0) M2[0] = ii*c1;
				else {
					size_t val1 = ((s1[ii] == s2[jj]) ? 0 : c2) + M1[jj - 1];
					M2[jj]= min(val1, min(M1[jj], M2[jj - 1]) + c1);
				}
			}
			tmpM = M1; M1 = M2; M2 = tmpM;
		}
		size_t re = M1[s2len - 1];
		delete[] M1, M2;
		return re;
	}
	static size_t StrDif(const std::string& s1, const std::string& s2) {
		return StrDif(s1.c_str(), s1.length(), s2.c_str(), s2.length());
	}

	//����ظ��ִ�����������ֵ�λ�úͳ���
	static std::pair<size_t, size_t> LongestSubStrWithoutDup(const char* s, size_t len) {
		assert(len && s!=NULL);
		size_t positions[256];//ÿ���ַ���һ�γ��ֵ�λ��
		for (size_t ii = 0; ii < len; ++ii) positions[ii] = len;//��ʼ��Ϊlen����ʾû����
		size_t maxLen = 0, maxPos = 0;//����ִ����Ⱥ�λ��
		size_t curLen = 0, curPos = 0;//��ǰ���ظ��ִ��ĳ��Ⱥ�λ��
		for (size_t ii = 0; ii < len; ++ii) {
			size_t &prePos = positions[s[ii]];
			if (prePos == len || (ii - prePos) > curLen) ++curLen;
			else {
				if (curLen > maxLen) { maxLen = curLen;	maxPos = curPos; }
				curLen = ii - prePos;
				curPos = prePos + 1;
			}
			prePos = ii;
		}
		if (curLen > maxLen) { maxLen = curLen;	maxPos = curPos; }
		return std::pair<size_t, size_t>(maxPos, maxLen);
	}
	static std::pair<size_t, size_t> LongestSubStrWithoutDup(const std::string& s) {
		return LongestSubStrWithoutDup(s.c_str(), s.length());
	}



}


