#pragma once
#include <ytrpsf/RPSF_Module/RPSF_CommonNode/Msg.h>

namespace rpsf {
	//����ϵͳ��Ϣ�����ṩ��������ã�һ�����ڴ��ݸ��ֽṹ��
	class rpsfSys {
		T_CLASS_SERIALIZE(&m_sysMsgType)
	public:
		//ϵͳ��Ϣ����
		enum {
			RPSF_SYS_NEWNODE
		};
		rpsfSys() :m_msgType(MsgType::RPSF_SYS | handleType::RPSF_EMCY) {

		}

		uint8_t m_sysMsgType;//��Ϣ����


		std::map<std::string, ytlib::shared_buf> map_datas;//����,���֧��255��

		uint8_t m_msgType;
	};
}


