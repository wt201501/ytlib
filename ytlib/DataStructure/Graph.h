#pragma once
#include <ytlib/Common/Util.h>
#include <ytlib/LightMath/Matrix.h>
#include <map>
#include <list>
#include <vector>

//ģ�廯��ͼ��һЩ���ߡ�todo��������
//����ʹ������ָ�룬��Ϊ�����໥ָ�������
//ʹ��ʱӦ�����еĽڵ������ָ�����һ��list��vector��
//ÿ���ڵ�ָ����һ���ڵ����ֻ��һ����
namespace ytlib {

	//һ���бߵ�Ȩ�ص�����ͼ�Ľڵ�
	template<typename T, typename sideType = uint32_t>
	class Graph {
	public:
		Graph():visited(false){}
		~Graph() {}
		Graph(const T& _obj):obj(_obj), visited(false) {}

		T obj;
		std::map<Graph<T, sideType>*, sideType> sides;//���Լ���Ȩ��
		//һЩΪ�˱��������õı�־λ
		bool visited;

	};
	//һЩ��ͼ�Ļ�������

	//����ͼ�Ľڵ���룬����Ϊ��������ڵ㡢Ŀ��ڵ㡢��Ȩֵ
	template<typename T, typename sideType = uint32_t>
	void insertGraphNode(Graph<T, sideType>& val,const Graph<T, sideType>& targt, sideType side) {
		val.sides[&targt] = side;
	}

	//����ͼ�Ľڵ����
	template<typename T, typename sideType = uint32_t>
	void connectGraphNode(Graph<T, sideType>& val, Graph<T, sideType>& targt, sideType side) {
		val.sides[&targt] = side;
		targt.sides[&val] = side;
	}

	//ͼ�ĸ���
	template<typename T, typename sideType = uint32_t>
	std::vector<Graph<T, sideType>*> copyGraph(const std::vector<Graph<T, sideType>*>& vec) {
		std::vector<Graph<T, sideType>*> re;



		return re;
	}


	//�����ڽӾ���
	template<typename T, typename sideType = uint32_t>
	Basic_Matrix<sideType> createAdjMatrix(const std::vector<Graph<T, sideType>*>& vec) {
		Basic_Matrix<sideType> M;

		return std::move(M);
	}

	//һЩͼ�Ļ����㷨

	//DFS������֮ǰӦȷ�����нڵ��visited�Ѿ�������Ϊfalse
	template<typename T, typename sideType = uint32_t>
	void DFS(Graph<T, sideType>& val, std::vector<Graph<T, sideType>*>& vec) {
		vec.push_back(&val);
		val.visited = true;
		for (typename std::map<Graph<T, sideType>*, sideType>::iterator itr = val.sides.begin(); itr != val.sides.end(); ++itr) {
			if (itr->first != NULL && !(itr->first->visited)) DFS(*itr->first, vec);
		}
	}

	//BFS������֮ǰӦȷ�����нڵ��visited�Ѿ�������Ϊfalse
	template<typename T, typename sideType = uint32_t>
	void BFS(Graph<T, sideType>& val, std::vector<Graph<T, sideType>*>& vec) {
		if (!val.visited) {
			//��ʼ�ڵ�
			vec.push_back(&val);
			val.visited = true;
		}
		std::vector<Graph<T, sideType>*> tmpvec;
		for (typename std::map<Graph<T, sideType>*, sideType>::iterator itr = val.sides.begin(); itr != val.sides.end(); ++itr) {
			if (itr->first != NULL && !(itr->first->visited)) {
				vec.push_back(itr->first);
				itr->first->visited = true;
				tmpvec.push_back(itr->first);
			}
		}
		size_t len = tmpvec.size();
		for (size_t ii = 0; ii < len; ++ii) {
			BFS(*tmpvec[ii], vec);
		}

	}


	//dijkstra�㷨����һ���ڵ㵽�����ڵ�����·��
	template<typename T, typename sideType = uint32_t>
	std::vector<sideType> dijkstra(Graph<T, sideType>& beginNode,const std::vector<Graph<T, sideType>*>& vec) {
		std::vector<sideType> re;


		return re;
	}
	//floyd�㷨�������нڵ㵽�������нڵ�����·��
	template<typename T, typename sideType = uint32_t>
	Basic_Matrix<sideType> floyd(const std::vector<Graph<T, sideType>*>& vec) {
		Basic_Matrix<sideType> M;

		return M;
	}


	//�����
	template<typename T, typename sideType = uint32_t>
	sideType maxFlow(Graph<T, sideType>& beginNode, Graph<T, sideType>& endNode) {
		sideType re = 0;


		return re;
	}

	//Kruskal�㷨����С���������ӱ߷������������vector�Ͻ����޸ģ����ظ��ڵ��ָ��
	template<typename T, typename sideType = uint32_t>
	Graph<T, sideType>* kruskal(std::vector<Graph<T, sideType>*>& vec) {
		
	}


	//Prim�㷨����С���������ӵ㷨�����������vector�Ͻ����޸ģ����ظ��ڵ��ָ��
	template<typename T, typename sideType = uint32_t>
	Graph<T, sideType>* prim(std::vector<Graph<T, sideType>*>& vec) {
		
	}

	//A*

}


