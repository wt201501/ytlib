#include <ytlib/Common/Error.h>

namespace ytlib
{
	static const tchar* gErrorMessages[ER_ERROR_COUNT] =
	{
		T_TEXT("����successful"),
		T_TEXT("δ֪�쳣"),

		T_TEXT("���л�����"),
		T_TEXT("�����л�����"),

		T_TEXT("�ļ�������"),
		T_TEXT("�Ƿ�����"),
		T_TEXT("�Ƿ��ļ�����"),
		T_TEXT("�Ƿ��ļ�"),
		T_TEXT("��ʼ���ļ�ʧ��"),
		T_TEXT("�����ļ�ʧ��"),
		T_TEXT("�����ļ�ʧ��")
	};

	const tchar* GetErrorMessage(Error err0) {
		int32_t err = static_cast<int32_t>(err0);
		assert(err >= 0 && err < ER_ERROR_COUNT);
		const tchar* msg = gErrorMessages[err];
		assert(msg && msg[0]);
		return msg;
	}


	Exception::Exception(Error err)
		: m_message()
		, m_errorcode(err) {

	}

	Exception::Exception(const tstring& msg)
		: m_message(msg)
		, m_errorcode(ER_ERROR_UNKNOWN_EXCEPTION) {

	}

	Exception::~Exception(void) throw() {

	}

	const char* Exception::what() const throw() {
		if (m_message.empty()) {
			return T_TSTRING_TO_STRING(std::string(GetErrorMessage(m_errorcode))).c_str();
		}
		else {
			return T_TSTRING_TO_STRING(m_message).c_str();
		}
	}

}