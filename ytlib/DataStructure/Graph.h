#pragma once
#include <ytlib/Common/Util.h>
#include <ytlib/LightMath/Matrix.h>
#include <map>
#include <list>
#include <vector>

//todo��������
//ģ�廯��ͼ��һЩ���ߡ�ֻ����Ϊ����ѧϰ֮�ã�ʵ�ʹ�������Ҫʱ��������ο�Դ����иĶ�
//����ʹ������ָ�룬��Ϊ�����໥ָ�������
//ʹ��ʱӦ�����еĽڵ��ָ�����һ��list��vector��
//ÿ���ڵ�ָ����һ���ڵ����ֻ��һ����
namespace ytlib {
	//һ���бߵ�Ȩ�ص�����ͼ�Ľڵ�
	typedef double g_sideType;
	typedef Basic_Matrix<g_sideType> g_sideMatrix;

	template<typename T>
	class Graph {
	public:
		
		Graph():visited(false){}
		~Graph() {}
		Graph(const T& _obj) :obj(_obj), visited(false) {}

		T obj; 
		std::map<Graph<T>*, g_sideType> sides;//���Լ���Ȩ�ء�Ȩ��Ĭ��Ϊdouble
		mutable bool visited;//���ڱ���
	};
	//һЩ��ͼ�Ļ�������

	//����ͼ�Ľڵ���룬����Ϊ��������ڵ㡢Ŀ��ڵ㡢��Ȩֵ
	template<typename T>
	void insertGraphNode(Graph<T>& val,Graph<T>& targt, g_sideType side) {
		val.sides[&targt] = side;
	} 

	//����ͼ�Ľڵ����
	template<typename T>
	void connectGraphNode(Graph<T>& val, Graph<T>& targt, g_sideType side) {
		val.sides[&targt] = side;
		targt.sides[&val] = side;
	}

	//�ж��Ƿ�������ͼ
	template<typename T>
	bool isUndiGraph(const std::vector<Graph<T>*>& vec) {
		for (uint32_t ii = 0; ii < vec.size(); ++ii) {
			for (auto itr = vec[ii]->sides.begin(); itr != vec[ii]->sides.end(); ++itr) {
				auto itr2 = itr->first->sides.find(vec[ii]);
				if (itr2 == itr->first->sides.end()) return false;
				if (itr2->second != itr->second) return false;
			}
		}
		return true;
	}

	//һЩͼ�Ļ����㷨

	//������������ȡ�ڵ���vector�е��±�
	template<typename T>
	inline size_t getPos(const Graph<T>* p, const std::vector<Graph<T>*>& vec) {
		size_t pos = find(vec.begin(), vec.end(), p) - vec.begin();
		assert(pos < vec.size());
		return pos;
	}

	//���������������־λ
	template<typename T>
	inline void clearFlag(const std::vector<Graph<T>*>& vec) {
		for (uint32_t ii = 0; ii < vec.size(); ++ii) vec[ii]->visited = false;
	}
	//�����������ͷ��ڴ�
	template<typename T>
	inline void releaseGraphVec(std::vector<Graph<T>*>& vec) {
		for (uint32_t ii = 0; ii < vec.size(); ++ii) delete vec[ii];
	}
	
	//�����ڽӾ���M.val[i][j]��ʾ�Ӷ���vec[i]����������vec[j]��ֱ�Ӿ��룬-1ֵ��ʾ��ֱ������
	template<typename T>
	g_sideMatrix createAdjMatrix(const std::vector<Graph<T>*>& vec) {
		size_t Vnum = vec.size();
		g_sideMatrix M(Vnum, Vnum);
		M.setVal(-1);//-1��ʾ������
		for (size_t ii = 0; ii < Vnum; ++ii) {
			M.val[ii][ii] = 0;
			for (auto itr = vec[ii]->sides.begin(); itr != vec[ii]->sides.end(); ++itr) {
				size_t pos = getPos(itr->first, vec);
				M.val[ii][pos] = itr->second;
			}
		}
		return M;
	}

	//ͼ�ĸ���
	template<typename T>
	std::vector<Graph<T>*> copyGraph(const std::vector<Graph<T>*>& vec) {
		std::vector<Graph<T>*> re;
		size_t len = vec.size();
		for (size_t ii = 0; ii < len; ++ii) {
			re.push_back(new Graph<T>(vec[ii]->obj));
		}
		for (size_t ii = 0; ii < len; ++ii) {
			for (auto itr = vec[ii]->sides.begin(); itr != vec[ii]->sides.end(); ++itr) {
				size_t pos = getPos(itr->first, vec);
				re[ii]->sides.insert(std::pair<Graph<T>*, g_sideType>(re[pos], itr->second));
			}
		}
		return re;
	}


	//DFS������֮ǰӦȷ�����нڵ��visited�Ѿ�������Ϊfalse
	template<typename T>
	void DFS(Graph<T>& val, std::vector<Graph<T>*>& vec) {
		vec.push_back(&val);//���ϲ㱣֤�˽ڵ�û�б�����
		val.visited = true;
		for (auto itr = val.sides.begin(); itr != val.sides.end(); ++itr) {
			if (!(itr->first->visited)) DFS(*itr->first, vec);
		}
	}

	//BFS������֮ǰӦȷ�����нڵ��visited�Ѿ�������Ϊfalse
	template<typename T>
	void BFS(Graph<T>& val, std::vector<Graph<T>*>& vec) {
		if (!val.visited) {
			//��ʼ�ڵ�
			vec.push_back(&val);
			val.visited = true;
		}
		std::vector<Graph<T>*> tmpvec;
		for (auto itr = val.sides.begin(); itr != val.sides.end(); ++itr) {
			if (!(itr->first->visited)) {
				itr->first->visited = true;
				vec.push_back(itr->first);
				tmpvec.push_back(itr->first);
			}
		}
		size_t len = tmpvec.size();
		for (size_t ii = 0; ii < len; ++ii) {
			BFS(*tmpvec[ii], vec);
		}

	}

	

	//dijkstra�㷨����һ���ڵ㵽�����ڵ�����·�������ؾ��������·�����顣��ֹ��Ȩ��
	template<typename T>
	std::pair<std::vector<g_sideType>, std::vector<int32_t> > dijkstra(const Graph<T>& beginNode,const std::vector<Graph<T>*>& vec) {
		size_t len = vec.size();
		std::vector<g_sideType> re(len, -1);
		std::vector<uint8_t> flag(len, 0);
		std::vector<int32_t> path(len, -1);

		size_t curPos = getPos(&beginNode, vec), nextPos = curPos;
		re[curPos] = 0;
		path[curPos] = curPos;
		do {
			curPos = nextPos;
			flag[curPos] = 1;
			for (auto itr = vec[curPos]->sides.begin(); itr != vec[curPos]->sides.end(); ++itr) {
				size_t pos = find(vec.begin(), vec.end(), itr->first) - vec.begin();
				if (flag[pos] == 0) {
					g_sideType sideLen = ((re[curPos] < 0) ? 0 : re[curPos]) + itr->second;
					if (re[pos] < 0 || re[pos]>sideLen) {
						re[pos] = sideLen;
						path[pos] = curPos;
					}
				}
			}
			g_sideType minLen = -1;
			for (size_t ii = 0; ii < len; ++ii) {
				if (flag[ii] == 0 && re[ii]>=0 && (minLen<0 || minLen>re[ii])) {
					minLen = re[ii];
					nextPos = ii;
				}
			}			
		} while (nextPos != curPos);
		return std::pair<std::vector<g_sideType>, std::vector<int32_t> >(std::move(re), std::move(path));
	}
	//����dijkstra���ص�·���������ض��ڵ㵽��һ���ڵ�����·�������ص��ǵ��Ƶ�·��
	static std::vector<int32_t> dijkstraPath(int32_t dstIdx,const std::vector<int32_t>& path) {
		std::vector<int32_t> re;
		assert(path[dstIdx] >= 0);
		do {
			dstIdx = path[dstIdx];
			re.push_back(dstIdx);
		} while (dstIdx != path[dstIdx]);
		return re;
	}

	//floyd�㷨�������нڵ㵽�������нڵ�����·�������ؾ�������·������
	template<typename T>
	std::pair<g_sideMatrix, Matrix_i> floyd(const std::vector<Graph<T>*>& vec) {
		size_t len = vec.size();
		g_sideMatrix distanceM = createAdjMatrix(vec);
		Matrix_i pathM(len, len);
		pathM.setVal(-1);
		
		for (size_t ii = 0; ii < len; ++ii) {
			for (size_t jj = 0; jj < len; ++jj) {
				if (distanceM.val[ii][jj] >= 0)	pathM.val[ii][jj] = ii;
			}
		}		
		for (size_t kk = 0; kk < len; ++kk) {
			for (size_t ii = 0; ii < len; ++ii) {
				if (ii == kk) continue;
				for (size_t jj = 0; jj < len; ++jj) {
					if (ii == jj || jj == kk) continue;
					if (distanceM.val[ii][kk] >= 0 && distanceM.val[kk][jj] >= 0) {
						g_sideType d = distanceM.val[ii][kk] + distanceM.val[kk][jj];
						if (distanceM.val[ii][jj]<0 || distanceM.val[ii][jj]>d) {
							distanceM.val[ii][jj] = d;
							pathM.val[ii][jj] = kk;
						}
					}
				}
			}
		}
		return std::pair<g_sideMatrix, Matrix_i>(std::move(distanceM), std::move(pathM));
	}

	//����floyd���ص�·��������һ���ڵ㵽��һ���ڵ�����·�������ص������Ƶ�·��
	static std::vector<int32_t> floydPath(int32_t srcIdx, int32_t dstIdx, const Matrix_i& path) {
		std::vector<int32_t> re;
		assert(path.val[srcIdx][dstIdx]>=0);
		re.push_back(srcIdx);
		do {
			srcIdx = path.val[srcIdx][dstIdx];
			re.push_back(srcIdx);
		} while (srcIdx != path.val[srcIdx][dstIdx]);
		return re;
	}


	//���������Ȩֵ��Ϊ����ֵ
	template<typename T>
	g_sideType maxFlow(Graph<T>& beginNode, Graph<T>& endNode) {
		g_sideType re = 0;


		return re;
	}

	//Kruskal�㷨����С���������ӱ߷������������vector�Ͻ����޸ģ����ظ��ڵ��ָ��
	template<typename T>
	Graph<T>* kruskal(std::vector<Graph<T>*>& vec) {
		
	}


	//Prim�㷨����С���������ӵ㷨�����������vector�Ͻ����޸ģ����ظ��ڵ��ָ��
	template<typename T>
	Graph<T>* prim(std::vector<Graph<T>*>& vec) {
		
	}

	//A*

}


