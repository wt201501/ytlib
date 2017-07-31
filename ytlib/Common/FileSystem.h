#pragma once

#include <ytlib/Common/Util.h>
#include <ytlib/Common/TString.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>

namespace ytlib
{
	//��װ��һЩboost�ⲻֱ���ṩ�ġ�boost���е�ֱ�ӵ���boost���
#if defined(UNICODE)
	typedef boost::filesystem::wpath tpath;
	typedef boost::xpressive::wsregex_compiler tsregex_compiler;
	typedef boost::filesystem::wrecursive_directory_iterator trecursive_directory_iterator;
#else
	typedef boost::filesystem::path tpath;
	typedef boost::xpressive::sregex_compiler tsregex_compiler;
	typedef boost::filesystem::recursive_directory_iterator trecursive_directory_iterator;
#endif


	//��õ�ǰ·������ִ���ļ�����Ŀ¼��
	static tpath tGetCurrentPath(void) {
#if defined(_WIN32)

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

		TCHAR szBuffer[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szBuffer, MAX_PATH);
		return tpath(szBuffer).parent_path();
#else
		return boost::filesystem::initial_path<tpath>();
#endif
	}
	
	//��þ���·��
	static tpath tGetAbsolutePath(const tpath& p) {
		if (p.is_absolute())
			return p;
		else
			return (tGetCurrentPath() / p);
	}



}

