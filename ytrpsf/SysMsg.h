#pragma once
#include <ytrpsf/Plugin_Bus_Interface.h>

namespace rpsf {
	//�������ϵͳ��Ϣ���������͵�ϵͳ��Ϣ���������ݵ������л���buff�У��յ�ʱ�ٷ����л�����

	//ϵͳ��Ϣ���͡����256��
	enum SysMsgType {
		SYS_TEST1,
		SYS_TEST2,
		SYS_COUNT	//������ͬʱҲ����Чֵ
	};

	class rpsfSysMsg : public rpsfPackage {
	public:
		rpsfSysMsg(){}
		rpsfSysMsg(SysMsgType sysMsgType_):m_sysMsgType(sysMsgType_){}
		SysMsgType m_sysMsgType;
	};



}


