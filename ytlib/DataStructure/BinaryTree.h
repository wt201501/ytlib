#pragma once
#include <ytlib/Common/Util.h>
#include <memory>
#include <vector>


namespace ytlib {

	//һ��BinTreeNodeʵ����ʾһ���������ڵ㣬Ҳ��ʾ�Դ˽ڵ�Ϊ���ڵ��һ�ö�����
	//������ڵ㱻��������ô�������������ӽڵ㽫�������������ӽڵ������������ָ��ָ��
	template<typename T>
	class BinTreeNode : public std::enable_shared_from_this<BinTreeNode<T> > {
	private:
		typedef std::shared_ptr<BinTreeNode<T> > nodePtr;
	public:
		BinTreeNode(){}
		virtual ~BinTreeNode(){}

		BinTreeNode(const T& _obj) :obj(_obj) {

		}


		T obj;
		BinTreeNode<T>* pf;//���ڵ㡣���ڵ㲻��ʹ������ָ�룬��������ѭ������
		nodePtr pl;//���ӽڵ�
		nodePtr pr;//���ӽڵ�

		//��һ���ڵ���Ϊ��/���ӽڵ㣨��ԭ��/���ӽڵ�Ͽ�������Ľڵ�����ԭ���ڵ�Ͽ���
		void setLChild(nodePtr& pnd) {
			assert(pnd);
			pnd->pf = this;
			pl = pnd;
		}
		void setRChild(nodePtr& pnd) {
			assert(pnd);
			pnd->pf = this;
			pr = pnd;
		}

		//����/�������Ͽ�
		void breakLChild() {
			pl->pf = NULL;
			pl.reset();
		}
		void breakRChild() {
			pr->pf = NULL;
			pr.reset();
		}


		//�ж��Ǹ��ڵ����ڵ㻹���ҽڵ㡣true��ʾ��ʹ��ǰӦ��鸸�ڵ��Ƿ�Ϊ��
		bool getLR() {
			assert(pf != NULL);
			if (this == pf->pl.get()) return true;
			if (this == pf->pr.get()) return false;
			assert(0);//���Ǹ��ڵ�����ҽڵ㡣����
			return true;
		}

		//��ȡ��ȣ����ڵ����Ϊ0
		virtual size_t getDepth() {
			BinTreeNode<T>* tmp = pf;
			size_t count = 0;
			while (tmp!=NULL) {
				++count;
				tmp = tmp->pf;
			}
			return count;
		}

		//��ȡ�߶ȣ�Ҷ�ӽڵ�߶�Ϊ1
		virtual size_t getHeight() {
			size_t lh = 0, rh = 0;
			if (pl) lh = pl->getHeight();
			if (pr) rh = pr->getHeight();

			return max(lh, rh) + 1;

		}

		//�Ե�ǰ�ڵ�Ϊ���ڵ㣬ǰ�����������һ��ָ�����顣�Ե�ǰ�ڵ�Ϊ���ڵ�
		void DLR(std::vector<nodePtr>& vec) {
			vec.push_back(shared_from_this());
			if (pl) pl->DLR(vec);
			if (pr) pr->DLR(vec);
		}

		//�������
		void LDR(std::vector<nodePtr>& vec) {
			if (pl) pl->LDR(vec);
			vec.push_back(shared_from_this());
			if (pr) pr->LDR(vec);
		}

		//��������
		void LRD(std::vector<nodePtr>& vec) {
			if (pl) pl->LRD(vec);
			if (pr) pr->LRD(vec);
			vec.push_back(shared_from_this());
		}

	};

	//�����������T��Ҫ֧�ֱȽ�����
	template<typename T>
	class BinSearchTreeNode : public BinTreeNode<T> {
	private:
		typedef std::shared_ptr<BinSearchTreeNode<T> > BSTNodePtr;
	public:
		BinSearchTreeNode():BinTreeNode<T>(){}
		virtual ~BinSearchTreeNode() {}

		BinSearchTreeNode(const T& _obj) :BinTreeNode<T>(_obj) {

		}

		//��ǰ�ڵ�Ϊ���ڵ�Ķ���������в���һ���ڵ�
		void insert(BSTNodePtr& ndptr) {
			assert(ndptr);
			if (ndptr->obj < obj) {
				if (pl) pl->insert(ndptr);
				else setLChild(ndptr);
			}
			else {
				if (pr) pr->insert(ndptr);
				else setRChild(ndptr);
			}
		}

		//ɾ����ǰ�ڵ㡣
		void erase() {
			
			if (!pl && !pr) {
				//���Ҷ�Ϊ�գ�ΪҶ�ӽڵ�
				if (pf == NULL) return;
				if (getLR) pf->breakLChild();//todo:�����������û᲻��������
				else pf->breakRChild();

			}
			else if (pl && !pr) {
				//ֻ��������
				if (pf == NULL) breakLChild();
				else {
					pl->pf = pf;

				}

			}
			else if (!pl && pr) {
				//ֻ��������


			}
			else {

			}


		}

	};



}


