#include <ytlib/Common/FileSystem.h>


namespace ytlib 
{

	//��õ�ǰ·������ִ���ļ�����Ŀ¼��
	tpath tGetCurrentPath(void) {
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
	tpath tGetAbsolutePath(const tpath& p) {
		if (p.is_absolute())
			return p;
		else
			return (tGetCurrentPath() / p);
	}


}