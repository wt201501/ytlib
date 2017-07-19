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
	tpath tGetCurrentPath(void);
	
	//��þ���·��
	tpath tGetAbsolutePath(const tpath& p);

	//�ϲ�·��
	tpath tCombinePath(const tpath& p1, const tpath& p2);

}

