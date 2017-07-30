#include <ytlib/Common/Util.h>
#include <ytlib/SupportTools/Serialize.h>
#include <ytlib/SupportTools/UUID.h>
#include <ytlib/NetTools/TcpNetAdapter.h>
#include <ytlib/LogService/LoggerServer.h>

#include "mathtest.h"
#include "filetest.h"
#include "processtest.h"
#include "nettest.h"

#include <boost/date_time/posix_time/posix_time.hpp>  

using namespace std;
using namespace ytlib;

int32_t main(int32_t argc, char** argv) {
	printf_s("-------------------start-------------------\n");
	//tcout���������Ҫ����
	//���飺��ò�Ҫ�ڳ�����ʹ�����ģ�����
	//std::locale::global(std::locale(""));
	//wcout.imbue(locale(""));


	//boost::posix_time::ptime ticTime_global, tocTime_global;
	//ticTime_global = boost::posix_time::microsec_clock::universal_time();

	//tocTime_global = boost::posix_time::microsec_clock::universal_time(); 
	//std::cout << (tocTime_global - ticTime_global).ticks() << "us" << std::endl;

	
	




	test_TcpNetAdapter();
	
	LoggerServer l(55555);
	l.start();


	test_Complex();
	test_Matrix();
	test_Matrix_c();

	test_KeyValueFile();
	test_SerializeFile();
	test_XMLFile();
	test_PrjBase();

	test_QueueProcess();
	
	printf_s("******************end*******************\n");
	getchar();
	
}
