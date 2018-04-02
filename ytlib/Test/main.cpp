#include <ytlib/Common/Util.h>

#include "t_lmath.h"
#include "t_file.h"
#include "t_process.h"
#include "t_net.h"
#include "t_log.h"
#include "t_tools.h"
#include "t_stru.h"
#include "t_algs.h"

#include <boost/date_time/posix_time/posix_time.hpp>  

using namespace std;
using namespace ytlib;

class c1 {
public:
	c1():index(b++){

		YT_DEBUG_PRINTF("create:%d\n", index);

	}
	c1(const c1& val) :index(b++) {
		YT_DEBUG_PRINTF("copy:%d to %d\n", val.index, index);
	}
	c1(c1&& val):index(val.index) {
		YT_DEBUG_PRINTF("move:%d\n", index);
	}

	~c1() {
		YT_DEBUG_PRINTF("delete:%d\n", index);
	}

	int index;
	const int a=1;
	static int b;
	static const int c;

private:


};
int c1::b = 1;
const int c1::c = 1;

class c2 {
public:
	c2(){}
	//~c2(){}

	//c2(const c2& val) :mb1(val.mb1) {

	//}

	//c2(c2&& val):mb1(std::move(val.mb1)){
	//	
	//}


	c1 mb1;

};

int32_t main(int32_t argc, char** argv) {
	YT_DEBUG_PRINTF("-------------------start-------------------\n");
	//tcout输出中文需要设置
	//建议：最好不要在程序中使用中文！！！
	//std::locale::global(std::locale(""));
	//wcout.imbue(locale(""));

	vector<c1> vec;
	vec.reserve(10);
	vec.push_back(c1());

	c1 tmp1;
	vec.push_back(tmp1);


	vec.push_back(std::move(c1()));

	c1 tmp2;
	vec.push_back(std::move(tmp2));


	vector<c2> vec2;
	vec2.reserve(10);
	vec2.push_back(c2());

	c2 tmp21;
	vec2.push_back(tmp21);


	vec2.push_back(std::move(c2()));

	c2 tmp22;
	vec2.push_back(std::move(tmp22));

	//boost::posix_time::ptime ticTime_global, tocTime_global;
	//ticTime_global = boost::posix_time::microsec_clock::universal_time();

	//tocTime_global = boost::posix_time::microsec_clock::universal_time(); 
	//std::cout << (tocTime_global - ticTime_global).ticks() << "us" << std::endl;
	
	//setTime();
	//cout << getTime() << endl;
	test_graph();

	test_sort();
	test_kmp();
	test_strdif();
	test_lgsswd();

	test_avlt();
	test_brt();
	test_bst();
	test_bintree();
	test_heap();


	test_Serialize();
	test_SysInfoTools();
	test_NetLog();
	test_TcpNetAdapter();
	

	test_Complex();
	test_Matrix();
	test_Matrix_c();
	test_tools();

	test_KeyValueFile();
	test_SerializeFile();
	test_XMLFile();
	test_PrjBase();

	test_QueueProcess();
	
	printf("******************end*******************\n");
	getchar();
	
}
