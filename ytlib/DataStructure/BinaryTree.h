#pragma once
#include <ytlib/Common/Util.h>
#include <memory>
#include <vector>


namespace ytlib {

	//һ��BinTreeNodeʵ����ʾһ���������ڵ㣬Ҳ��ʾ�Դ˽ڵ�Ϊ���ڵ��һ�ö�����
	//������ڵ㱻��������ô�������������ӽڵ㽫�������������ӽڵ������������ָ��ָ��
	template<typename T>
	class BinTreeNode {
	private:
		typedef std::shared_ptr<BinTreeNode<T> > nodePtr;
	public:
		BinTreeNode():pf(NULL){}
		virtual ~BinTreeNode(){}

		BinTreeNode(const T& _obj) :obj(_obj), pf(NULL) {

		}

		T obj;
		BinTreeNode<T>* pf;//���ڵ㡣���ڵ㲻��ʹ������ָ�룬��������ѭ������
		nodePtr pl;//���ӽڵ�
		nodePtr pr;//���ӽڵ�

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
		bool getLR() const {
			assert(pf != NULL);
			if (this == pf->pl.get()) return true;
			if (this == pf->pr.get()) return false;
			assert(0);//���Ǹ��ڵ�����ҽڵ㡣����
			return true;
		}

		//��ȡ��ȣ����ڵ����Ϊ0
		size_t getDepth() const {
			BinTreeNode<T>* tmp = pf;
			size_t count = 0;
			while (tmp!=NULL) {
				++count;
				tmp = tmp->pf;
			}
			return count;
		}

		//��ȡ�߶ȣ�Ҷ�ӽڵ�߶�Ϊ1
		size_t getHeight() const {
			size_t lh = 0, rh = 0;
			if (pl) lh = pl->getHeight();
			if (pr) rh = pr->getHeight();

			return max(lh, rh) + 1;

		}
		//��ȡ���нڵ����
		size_t getNodeNum() const {
			size_t num = 1;
			if (pl) num += pl->getNodeNum();
			if (pr) num += pr->getNodeNum();
			return num;
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
		virtual void insert(BSTNodePtr& ndptr) {
			assert(ndptr);
			if (ndptr->obj < obj) {
				if (pl) static_cast<BinSearchTreeNode<T>*>(pl.get())->insert(ndptr);
				else setLChild(this,ndptr);
			}
			else {
				if (pr) static_cast<BinSearchTreeNode<T>*>(pr.get())->insert(ndptr);
				else setRChild(this, ndptr);
			}
		}

		//ɾ����ǰ�ڵ㡣
		virtual void erase() {
			
			if (!pl && !pr) {
				//���Ҷ�Ϊ�գ�ΪҶ�ӽڵ�
				if (pf == NULL) return;
				if (getLR()) pf->breakLChild();//todo:�����������û᲻��������
				else pf->breakRChild();

			}
			else if (pl && !pr) {
				//ֻ��������
				if (pf == NULL) breakLChild();
				else {
					pl->pf = pf;
					pf->pl = pl;
					pf = NULL;
					pl.reset();
				}

			}
			else if (!pl && pr) {
				//ֻ��������
				if (pf == NULL) breakRChild();
				else {
					pr->pf = pf;
					pf->pr = pr;
					pf = NULL;
					pr.reset();
				}

			}
			else {
				//����������ǰ��
				std::shared_ptr<BinTreeNode<T> > tmp = pl;
				if (tmp->pr) {
					//���ӽڵ������������ҵ���ǰ��
					while (tmp->pr) {
						tmp = tmp->pr;
					}
					
					tmp->pf->pr = tmp->pl;
					if (tmp->pl) tmp->pl->pf = tmp->pf;
					tmp->pl = pl; pl->pf = tmp.get();

				}

				tmp->pf = pf;
				tmp->pr = pr; pr->pf = tmp.get();
				if (pf) {
					if (getLR()) pf->pl = tmp;
					else pf->pr = tmp;
				}
				pf = NULL;
				pl.reset();
				pr.reset();

			}

		}

	};

#define AVLTPTR(sp) static_cast<AVLTreeNode<T>*>(sp.get())	//��BinTreeNodeָ��ת��ΪAVL����ָ��
	//AVL��
	template<typename T>
	class AVLTreeNode : public BinSearchTreeNode <T>{
	private:
		typedef std::shared_ptr<AVLTreeNode<T> > AVLTNodePtr;
	public:
		AVLTreeNode() :AVLTreeNode<T>(), hgt(1) {}
		virtual ~AVLTreeNode() {}

		AVLTreeNode(const T& _obj) :BinSearchTreeNode<T>(_obj), hgt(1) {

		}

		size_t hgt;//�ڵ�߶�

		virtual void insert(AVLTNodePtr& ndptr) {
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

		virtual void erase() {

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
		if (nd->pl) DLR<T>(std::static_pointer_cast<T>(nd->pl), vec);
		if (nd->pr) DLR<T>(std::static_pointer_cast<T>(nd->pr), vec);
	}
	//�������
	template<typename T>
	void LDR(std::shared_ptr<T>& nd, std::vector<std::shared_ptr<T> >& vec) {
		if (nd->pl) LDR<T>(std::static_pointer_cast<T>(nd->pl), vec);
		vec.push_back(nd);
		if (nd->pr) LDR<T>(std::static_pointer_cast<T>(nd->pr), vec);
	}
	//��������
	template<typename T>
	void LRD(std::shared_ptr<T>& nd, std::vector<std::shared_ptr<T> >& vec) {
		if (nd->pl) LRD<T>(std::static_pointer_cast<T>(nd->pl), vec);
		if (nd->pr) LRD<T>(std::static_pointer_cast<T>(nd->pr), vec);
		vec.push_back(nd);
	}

	//��һ���ڵ���Ϊ��/���ӽڵ㣨��ԭ��/���ӽڵ�Ͽ�������Ľڵ�����ԭ���ڵ�Ͽ���
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

}


