#pragma once
#include <ytrpsf/Plugin_Bus_Interface.h>
#include <boost/noncopyable.hpp>

//���ģ�������Ū
namespace rpsf {
	//����ͻ��ˣ�ʹ�����ߵ�ҵ��������������
	//��������һ����ͨ�ڵ㣬�ܹ���������ڵ������ֹͣ�����ܹ����Զ���Ĳ����̬��ӵ�����ڵ������������������ڵ�Ĳ������
	
	//���ļ������������ṩ���ͻ��˵Ľӿ�
	class Client : boost::noncopyable {
	public:
		//���߿��ƽӿڡ���ʼ����start��������ֹͣ
		Client();
		virtual ~Client();
		bool Init(const std::string& cfgpath);

		bool AddPlugin(IPlugin* p_plugin);//ҵ��ӿڡ��û����Լ�ʵ�ֵĲ����ӵ�������

		IBus* get_pIBus();
	private:
		void* p;
	};

}