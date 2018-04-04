#include "t_stru.h"
#include <iostream>

using namespace std;
namespace ytlib
{
	bool test_heap() {

		int data[10] = { 7,9,2,6,4,8,1,3,10,5 };
		Heap<int> h1(data, 10);
		for (int ii = 0; ii < 10; ++ii) {
			cout << h1.container[ii] << " ";
		}
		cout << endl;

		h1.sort();
		for (int ii = 0; ii < 10; ++ii) {
			cout << h1.container[ii] << " ";
		}
		cout << endl;


		int data2[15] = { 7,-5,9,-7,2,6,-6,4,8,1,3,-8,10,5, -9};
		h1.assign(data2, 15);
		h1.sort();

		for (int ii = 0; ii < 15; ++ii) {
			cout << h1.container[ii] << " ";
		}
		cout << endl;

		h1.push(-3);
		h1.push(-10);
		h1.push(11);
		for (int ii = 0; ii < 18; ++ii) {
			cout << h1.container[ii] << " ";
		}

		h1.pop();
		h1.pop();
		h1.pop();
		for (int ii = 0; ii < 15; ++ii) {
			cout << h1.container[ii] << " ";
		}
		cout << endl;



		return true;
	}

	bool test_bintree() {
		typedef BinTreeNode<int> mybt;
		typedef shared_ptr<mybt> mybtPtr;
		mybtPtr rootp = make_shared<mybt>(10);
		setLChild(rootp.get(), make_shared<mybt>(5));
		setRChild(rootp.get(), make_shared<mybt>(3));

		mybtPtr p = rootp->pl;
		setLChild(p.get(), make_shared<mybt>(1));
		setRChild(p.get(), make_shared<mybt>(7));

		p = p->pr;
		setRChild(p.get(), make_shared<mybt>(4));

		p = rootp->pr;
		setRChild(p.get(), make_shared<mybt>(8));
		p = p->pr;
		setLChild(p.get(), make_shared<mybt>(9));

		cout << getHeight(rootp.get()) << endl;
		cout << getDepth(p.get()) << endl;

		//ǰ�����
		vector<mybtPtr> vec;
		DLR(rootp,vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << " ";
		}
		cout << endl;

		//�������
		vec.clear();
		LDR(rootp,vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << " ";
		}
		cout << endl;

		//��������
		vec.clear();
		LRD(rootp,vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << " ";
		}
		cout << endl;



		return true;
	}

	bool test_bst() {
		typedef BinSearchTreeNode<int> mybst;
		typedef shared_ptr<mybst> mybstPtr;

		mybstPtr rootp = make_shared<mybst>(10);

		for (int ii = 0; ii < 20; ii += 2) {
			rootp->insert(make_shared<mybst>(ii));
		}

		vector<mybstPtr> vec;
		LDR(rootp,vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << " ";
		}
		cout << endl;

		vec.clear();
		DLR(rootp, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << " ";
		}
		cout << endl;

		return true;
	}

	bool test_avlt() {
		typedef AVLTreeNode<int> myavlt;
		typedef shared_ptr<myavlt> myavltPtr;

		myavltPtr rootp = make_shared<myavlt>(10);

		//����
		vector<myavltPtr> vec;
		for (int ii = 0; ii < 20; ii += 2) {
			rootp = rootp->insert(make_shared<myavlt>(ii));
			vec.clear();
			DLR(rootp, vec);
			for (size_t ii = 0; ii < vec.size(); ++ii) {
				cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
			}
			cout << endl;
		}
		vec.clear();
		LDR(rootp, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
		}
		cout << endl;

		vec.clear();
		DLR(rootp, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
		}
		cout << endl;

		//�ֲ����
		vec.clear();
		traByLevel(rootp, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
		}
		cout << endl;

		//ɾ��
		rootp = rootp->erase(100);
		vec.clear();
		DLR(rootp, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
		}
		cout << endl;

		rootp = rootp->erase(14);
		vec.clear();
		DLR(rootp, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
		}
		cout << endl;

		//ɾ�����ڵ�
		rootp = rootp->erase(rootp);
		vec.clear();
		DLR(rootp, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
		}
		cout << endl;

		rootp = rootp->erase(0);
		vec.clear();
		DLR(rootp, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
		}
		cout << endl;

		//ɾ���ҽڵ�
		rootp = rootp->erase(rootp->pr->pr);
		vec.clear();
		DLR(rootp, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
		}
		cout << endl;

		



		//���л��������л�
		vec.clear();
		SerializeTree(rootp, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			if (vec[ii]) cout << vec[ii]->obj << " ";
			else cout << "* ";
		}
		cout << endl;

		myavltPtr rootp2;
		DeserializeTree(rootp2, vec.begin());
		vec.clear();
		traByLevel(rootp2, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
		}
		cout << endl;

		//���ĸ���
		myavltPtr rootp3 = copyTree(rootp);
		vec.clear();
		traByLevel(rootp3, vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << "(" << vec[ii]->hgt << ") ";
		}
		cout << endl;

		return true;
	}

	bool test_brt() {


		return true;
	}


	bool test_graph() {
		typedef Graph<uint32_t> myGraph;

		vector<myGraph> myGraphVec;
		uint32_t num = 10;
		for (uint32_t ii = 0; ii < num; ++ii) {
			myGraphVec.push_back(myGraph(ii));
		}

		connectGraphNode<uint32_t, uint32_t>(myGraphVec[0], myGraphVec[1], 1);
		connectGraphNode<uint32_t, uint32_t>(myGraphVec[0], myGraphVec[5], 1);
		connectGraphNode<uint32_t, uint32_t>(myGraphVec[0], myGraphVec[9], 1);
		connectGraphNode<uint32_t, uint32_t>(myGraphVec[1], myGraphVec[2], 1);
		connectGraphNode<uint32_t, uint32_t>(myGraphVec[2], myGraphVec[8], 1);
		connectGraphNode<uint32_t, uint32_t>(myGraphVec[3], myGraphVec[4], 1);
		connectGraphNode<uint32_t, uint32_t>(myGraphVec[3], myGraphVec[7], 1);
		connectGraphNode<uint32_t, uint32_t>(myGraphVec[4], myGraphVec[5], 1);
		connectGraphNode<uint32_t, uint32_t>(myGraphVec[6], myGraphVec[7], 1);
		connectGraphNode<uint32_t, uint32_t>(myGraphVec[6], myGraphVec[8], 1);
		connectGraphNode<uint32_t, uint32_t>(myGraphVec[8], myGraphVec[9], 1);

		//dfs
		vector<myGraph*> vec;
		for (uint32_t ii = 0; ii < num; ++ii) myGraphVec[ii].visited = false;
		DFS(myGraphVec[0], vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << " ";
		}
		cout << endl;

		//bfs
		vec.clear();
		for (uint32_t ii = 0; ii < num; ++ii) myGraphVec[ii].visited = false;
		BFS(myGraphVec[0], vec);
		for (size_t ii = 0; ii < vec.size(); ++ii) {
			cout << vec[ii]->obj << " ";
		}
		cout << endl;




		return true;
	}
}