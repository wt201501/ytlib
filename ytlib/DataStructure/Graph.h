#pragma once
#include <ytlib/Common/Util.h>
#include <ytlib/LightMath/Matrix.h>
#include <map>
#include <list>


//ģ�廯��ͼ��һЩ���ߡ�todo��������
//����ʹ������ָ�룬��Ϊ�����໥ָ�������
//ʹ��ʱӦ�����еĽڵ����һ��list��
namespace ytlib {

	//һ���бߵ�Ȩ�ص�����ͼ�Ľڵ�
	template<typename T, typename sideType = uint32_t>
	class Graph {
	public:
		typedef std::list<Graph<T, sideType> > graphContainer;

		T obj;

		map<Graph<T, sideType>*, sideType> sides;//���Լ���Ȩ��

	};
	//һЩ��ͼ�Ļ�������

	//����ͼ�Ľڵ����




	//����ͼ�Ľڵ����



	//�����ڽӾ���





	//һЩͼ�Ļ����㷨

	//DFS


	//BFS



	//�Ͻ�˹�����㷨

	//��С������ 


	//A*

}


