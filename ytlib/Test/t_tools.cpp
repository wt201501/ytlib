#include "t_tools.h"
#include <ytlib/SupportTools/UUID.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
using namespace std;
namespace ytlib
{
	bool test_SysInfoTools() {
		for (uint32_t ii = 0; ii < 10; ++ii) {
			printf("cpu: %f\tmem:%f\n", GetCpuUsage(), GetMemUsage());
		}
		return true;
	}


	class test_a {
		T_CLASS_SERIALIZE(&s&a&ps&ps2)
	public:

		string s;
		uint32_t a;
		boost::shared_ptr<test_a> ps;
		boost::shared_ptr<test_a> ps2;
		
	};


	bool test_Serialize() {
		test_a obj1;
		obj1.s = "dddd";
		obj1.a = 100;
		string re;
		Serialize(obj1, re, SerializeType::BinaryType);

		test_a obj2;
		Deserialize(obj2, re, SerializeType::BinaryType);

		return true;
	}

	//�������ֻ������ѧϰstl������㷨
	bool test_stl() {

		int re = 0;
		vector<int> v1{ 5,4,8,2,1,3,9,7,6 };//δ��������1
		
		re = accumulate(v1.begin(), v1.end(), 0);
		cout << re << endl;




		return true;
	}

}