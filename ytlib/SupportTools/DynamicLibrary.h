#pragma once

#include <ytlib/Common/Util.h>
#include <ytlib/Common/TString.h>
#include <map>
#include <memory>

namespace ytlib
{
	/*
	��̬������ʱ���������ַ�ʽ��
	1���õ�ʱ�����ӣ����꼴free������ʹ��DynamicLibrary�༴��
	2�������ڲ��һ����һֱ�����ž������ʱҪ�ã�ʹ��DynamicLibraryContainer��
	*/
#if defined(_WIN32)
	typedef FARPROC SYMBOL_TYPE;
#else
	typedef void* SYMBOL_TYPE;
#endif

	//��̬���ӿ��װ��,�ṩ��̬�����ͷŶ�̬���ӿⲢ��ȡ������ַ������
	class DynamicLibrary
	{
	public:
		DynamicLibrary(void):m_hnd(NULL){}
		DynamicLibrary(const tstring& name) :m_hnd(NULL) {
			Load(name);
		}
		~DynamicLibrary(void) {
			Free();
		}
		operator bool() {
			return (NULL != m_hnd);
		}
		// ��ȡ������ַ
		SYMBOL_TYPE GetSymbol(const tstring& name) {
			assert(NULL != m_hnd);

#if defined(_WIN32) 
			SYMBOL_TYPE symbol = ::GetProcAddress(m_hnd, T_TSTRING_TO_STRING(name).c_str());
#else
			SYMBOL_TYPE symbol = dlsym(m_hnd, T_TSTRING_TO_STRING(name).c_str());
#endif
			return symbol;
		}
		//��ȡ��̬���ӿ�����
		const tstring& GetLibraryName(void) const{ return m_libname; }

		//�������Ƽ��ض�̬���ӿ�
		bool Load(const tstring& libname) {
			m_libname = libname;
			Free();
#if defined(_WIN32)
			if ((m_libname.length()>4) && (m_libname.substr(m_libname.length() - 4, 4) != T_TEXT(".dll"))) {
				m_libname = m_libname + T_TEXT(".dll");
			}
			m_hnd = ::LoadLibrary(m_libname.c_str());
#else
			if (m_libname.substr(0, 3) != T_TEXT("lib")) {
				m_libname = T_TEXT("lib") + m_libname;
			}

			if (m_libname.substr(m_libname.length() - 3, 3) != T_TEXT(".so")) {
				m_libname = m_libname + T_TEXT(".so");
			}

			int32_t flags = RTLD_NOW | RTLD_GLOBAL;
			m_hnd = dlopen(T_TSTRING_TO_STRING(m_libname).c_str(), flags);
			if (NULL == m_hnd) {
				fprintf(stderr, "%s\n", dlerror());
			}
#endif
			return (NULL != m_hnd);
		}
		//�ͷŶ�̬���ӿ�
		void Free(void) {
			if (NULL != m_hnd) {
#if defined(_WIN32)
				::FreeLibrary(m_hnd);
#else
				dlclose(m_hnd);
#endif
				m_hnd = NULL;
			}
		}

	private:
#if defined(_WIN32)
		HINSTANCE m_hnd;							// Windowsƽ̨�Ķ�̬���ӿ���
#else
		void* m_hnd;								// ��Unixƽ̨�Ķ�̬���ӿ�ָ��
#endif
		tstring m_libname;							// ��̬���ӿ�����
	};

	
}


