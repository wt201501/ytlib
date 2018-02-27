#include <ytrpsf/CenterNode.h>

using namespace ytlib;
using namespace rpsf;
using namespace std;

//��򵥵����������Ľڵ�
int32_t main(int32_t argc, char** argv) {
	tcout << T_TEXT("---------RPSF Center Node---------") << endl;
	//��ȡ�����ļ�·��
	string cfgpath;
	if (argc == 1) {
		cfgpath = (tGetCurrentPath() / T_TEXT("CenterNode.xcfg")).string<string>();
	}
	else if (argc == 2) {
		cfgpath = std::string(argv[1]);
	}
	else {
		tcout << T_TEXT("argument error! press enter to exit.") << endl;
		getchar();
		return 0;
	}
	
	//�����ڵ�
	CenterNode nd;
	if (!nd.Init(cfgpath)) {
		tcout << T_TEXT("init error! press enter to exit.") << endl;
		getchar();
		return 0;
	}
	
	tcout << T_TEXT("start working! press enter to stop and exit.") << endl;
	getchar();
	return 0;
}
