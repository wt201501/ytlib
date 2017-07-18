#include <ytlib/Common/Error.h>

namespace wtlib
{
	static const tchar* gErrorMessages[ER_ERROR_COUNT] =
	{
		WT_TEXT("����successful"),
		WT_TEXT("δ֪�쳣"),

		WT_TEXT("���л�����"),
		WT_TEXT("�����л�����"),

		WT_TEXT("�ļ�������"),
		WT_TEXT("�Ƿ�����"),
		WT_TEXT("�Ƿ��ļ�����"),
		WT_TEXT("�Ƿ��ļ�"),
		WT_TEXT("��ʼ���ļ�ʧ��"),
		WT_TEXT("�����ļ�ʧ��"),
		WT_TEXT("�����ļ�ʧ��")
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
			return WT_TSTRING_TO_STRING(std::string(GetErrorMessage(m_errorcode))).c_str();
		}
		else {
			return WT_TSTRING_TO_STRING(m_message).c_str();
		}
	}

}