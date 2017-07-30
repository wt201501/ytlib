#pragma once
#include <boost/asio.hpp>


namespace ytlib
{
	/*
	��׼���ݰ����͹淶��
	step1���ȴ�һ����ͷ����8 byte��
		head: 2 byte
		tag: 2 byte
		size: 4 byte ��Ĭ��windows����С��
			num = byte1+byte2*256+byte3*65536+byte4*2^24
			byte1=num%256,byte2=(num/256) ...
	step2������size��byte������
	���ʹ�ý������Ļ�����Ҫ�ڷ������һ��������һ������head��tag = TCPEND1 + TCPEND2
	*/
	enum {
		TCPHEAD1 = 'Y',
		TCPHEAD2 = 'T',
		TCPEND1 = 'O',
		TCPEND2 = 'V'
	};

	//Ĭ��vs
	static void set_buf_from_num(char* p, uint32_t n) {
#ifdef _MSC_VER
		memcpy(p, &n, 4);
#else
		p[0] = char(n % 256); n /= 256;	p[1] = char(n % 256); n /= 256;
		p[2] = char(n % 256); n /= 256;	p[3] = char(n % 256);
#endif // _MSC_VER
	}

	static uint32_t get_num_from_buf(char* p) {
#ifdef _MSC_VER
		uint32_t n;	memcpy(&n, p, 4); return n;
#else
		return (p[0] + p[1] * 256 + p[2] * 65536 + p[3] * 256 * 65536);
#endif // _MSC_VER
	}

	//���˿��Ƿ���á�true˵������
	static bool checkPort(uint16_t port_) {
		boost::asio::io_service io;
		boost::asio::ip::tcp::socket sk(io);
		sk.open(boost::asio::ip::tcp::v4());
		boost::system::error_code err;
		sk.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_), err);
		if (err) return true;//���Ӳ��ϣ�˵��û�г����ڼ���
		sk.close();//����˵���Ѿ���ռ����
		return false;
	}


}