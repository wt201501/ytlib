#pragma once
#include <ytlib/Common/Util.h>
#include <memory>
#include <vector>


namespace ytlib {

	//ȫ��ʹ��ģ�廯�����ﲻ��ʹ�ü̳У���Ϊ�г�Ա���������͵�����ָ��
	//һ��BinTreeNodeʵ����ʾһ���������ڵ㣬Ҳ��ʾ�Դ˽ڵ�Ϊ���ڵ��һ�ö�����
	//������ڵ㱻��������ô�������������ӽڵ㽫�������������ӽڵ������������ָ��ָ��
	template<typename T>
	class BinTreeNode {
	private:
		typedef std::shared_ptr<BinTreeNode<T> > nodePtr;
	public:
		BinTreeNode():pf(NULL){}
		virtual ~BinTreeNode(){}
		BinTreeNode(const T& _obj) :obj(_obj), pf(NULL) {}

		T obj;
		BinTreeNode<T>* pf;//���ڵ㡣���ڵ㲻��ʹ������ָ�룬��������ѭ������
		nodePtr pl;//���ӽڵ�
		nodePtr pr;//���ӽڵ�

	};

	//�����������T��Ҫ֧�ֱȽ�����
	template<typename T>
	class BinSearchTreeNode {
	private:
		typedef std::shared_ptr<BinSearchTreeNode<T> > BSTNodePtr;
	public:
		BinSearchTreeNode() :pf(NULL) {}
		virtual ~BinSearchTreeNode() {}
		BinSearchTreeNode(const T& _obj) :obj(_obj), pf(NULL) {}

		T obj;
		BinSearchTreeNode<T>* pf;//���ڵ�
		BSTNodePtr pl;//���ӽڵ�
		BSTNodePtr pr;//���ӽڵ�

		//��ǰ�ڵ�Ϊ���ڵ�Ķ���������в���һ���ڵ�
		void insert(BSTNodePtr& ndptr) {
			assert(ndptr);
			if (ndptr->obj < obj) {
				if (pl) pl->insert(ndptr);
				else setLChild(this,ndptr);
			}
			else {
				if (pr) pr->insert(ndptr);
				else setRChild(this, ndptr);
			}
		}

		//ɾ����ǰ�ڵ㡣
		void erase() {
			if (!pl && !pr) {
				//���Ҷ�Ϊ�գ�ΪҶ�ӽڵ�
				if (pf == NULL) return;
				if (getLR(this)) breakLChild(pf);
				else breakRChild(pf);
			}
			else if (pl && !pr) {
				//ֻ��������
				if (pf == NULL) breakLChild(this);
				else {
					pl->pf = pf;
					pf->pl = pl;
					pf = NULL;pl.reset();
				}
			}
			else if (!pl && pr) {
				//ֻ��������
				if (pf == NULL) breakRChild(this);
				else {
					pr->pf = pf;
					pf->pr = pr;
					pf = NULL;pr.reset();
				}
			}
			else {
				//����������ǰ��
				BSTNodePtr tmp = pl;
				if (tmp->pr) {
					//���ӽڵ������������ҵ���ǰ��
					while (tmp->pr) tmp = tmp->pr;
					tmp->pf->pr = tmp->pl;
					if (tmp->pl) tmp->pl->pf = tmp->pf;
					tmp->pl = pl; pl->pf = tmp.get();
				}
				tmp->pf = pf;
				tmp->pr = pr; pr->pf = tmp.get();
				if (pf) {
					if (getLR(this)) pf->pl = tmp;
					else pf->pr = tmp;
				}
				pf = NULL;pl.reset();pr.reset();
			}
		}

	};

	//AVL��
	template<typename T>
	class AVLTreeNode {
	private:
		typedef std::shared_ptr<AVLTreeNode<T> > AVLTNodePtr;
	public:
		AVLTreeNode() :pf(NULL), hgt(1){}
		virtual ~AVLTreeNode() {}
		AVLTreeNode(const T& _obj) :obj(_obj), pf(NULL), hgt(1) {}

		T obj;
		AVLTreeNode<T>* pf;//���ڵ�
		AVLTNodePtr pl;//���ӽڵ�
		AVLTNodePtr pr;//���ӽڵ�

		size_t hgt;//�ڵ�߶�

		void insert(AVLTNodePtr& ndptr) {
			assert(ndptr);
			if (ndptr->obj < obj) {
				if (pl) { 
					AVLTPTR(pl)->insert(ndptr);

					size_t Rhgt = 0;
					if (pr) Rhgt = AVLTPTR(pr)->hgt;
					if (2 == AVLTPTR(pl)->hgt - Rhgt) {
						if (ndptr->obj < AVLTPTR(pl)->obj) {
							rotateL();
						}
						else {
							AVLTPTR(pr)->rotateR();
							rotateL();
						}
					}

				}
				else {
					setLChild(this, ndptr);
					ndptr->updateHeight();
				}



			}
			else {
				if (pr) AVLTPTR(pr)->insert(ndptr);
				else {
					setRChild(this, ndptr);
					ndptr->updateHeight();
				}
			}

		}

		void erase() {

		}

		//����ת��˳ʱ��
		void rotateL() {

		}
		//����ת����ʱ��
		void rotateR() {

		}

		//���ӽڵ����ϸ��¸߶�
		void updateHeight() {
			AVLTreeNode<T>* tmp = this;
			while (tmp->pf != NULL) {
				if (tmp->pf->hgt == (tmp->hgt + 1)) break;
				tmp->pf->hgt = (tmp->hgt + 1);
				tmp = tmp->pf;
			}
		}
	};

	//�����
	template<typename T>
	class BRTreeNode {

	};




	//�Ե�ǰ�ڵ�Ϊ���ڵ㣬ǰ�����������һ��ָ�����顣�Ե�ǰ�ڵ�Ϊ���ڵ�
	template<typename T>
	void DLR(std::shared_ptr<T>& nd, std::vector<std::shared_ptr<T> >& vec) {
		vec.push_back(nd);
		if (nd->pl) DLR(nd->pl, vec);
		if (nd->pr) DLR(nd->pr, vec);
	}
	//�������
	template<typename T>
	void LDR(std::shared_ptr<T>& nd, std::vector<std::shared_ptr<T> >& vec) {
		if (nd->pl) LDR(nd->pl, vec);
		vec.push_back(nd);
		if (nd->pr) LDR(nd->pr, vec);
	}
	//��������
	template<typename T>
	void LRD(std::shared_ptr<T>& nd, std::vector<std::shared_ptr<T> >& vec) {
		if (nd->pl) LRD(nd->pl, vec);
		if (nd->pr) LRD(nd->pr, vec);
		vec.push_back(nd);
	}

	//��ȡ��ȣ����ڵ����Ϊ0
	template<typename T>
	size_t getDepth(const T* pnode) {
		pnode = pnode->pf;
		size_t count = 0;
		while (pnode != NULL) {
			++count;
			pnode = pnode->pf;
		}
		return count;
	}

	//��ȡ�߶ȣ�Ҷ�ӽڵ�߶�Ϊ1
	template<typename T>
	size_t getHeight(const T* pnode) {
		size_t lh = 0, rh = 0;
		if (pnode->pl) lh = getHeight(pnode->pl.get());
		if (pnode->pr) rh = getHeight(pnode->pr.get());
		return max(lh, rh) + 1;
	}
	//��ȡ���нڵ����
	template<typename T>
	size_t getNodeNum(const T* pnode) {
		size_t num = 1;
		if (pnode->pl) num += getNodeNum(pnode->pl.get());
		if (pnode->pr) num += getNodeNum(pnode->pr.get());
		return num;
	}

	//��һ���ڵ���Ϊ��/���ӽڵ㣬��ԭ��/���ӽڵ�Ͽ�������Ľڵ�����ԭ���ڵ�Ͽ�
	template<typename T>
	void setLChild(T* pfather,std::shared_ptr<T>& pchild) {
		assert(pfather && pchild);
		pchild->pf = pfather;
		pfather->pl = pchild;
	}
	template<typename T>
	void setRChild(T* pfather, std::shared_ptr<T>& pchild) {
		assert(pfather && pchild);
		pchild->pf = pfather;
		pfather->pr = pchild;
	}

	//����/�������Ͽ�
	template<typename T>
	void breakLChild(T* pnode) {
		pnode->pl->pf = NULL;
		pnode->pl.reset();
	}
	template<typename T>
	void breakRChild(T* pnode) {
		pnode->pr->pf = NULL;
		pnode->pr.reset();
	}

	//�ж��Ǹ��ڵ����ڵ㻹���ҽڵ㡣true��ʾ��ʹ��ǰӦ��鸸�ڵ��Ƿ�Ϊ��
	template<typename T>
	bool getLR(const T* pnode) {
		assert(pnode->pf != NULL);
		if (pnode == pnode->pf->pl.get()) return true;
		if (pnode == pnode->pf->pr.get()) return false;
		assert(0);//���Ǹ��ڵ�����ҽڵ㡣����
		return true;
	}
}


