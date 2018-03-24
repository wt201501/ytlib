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
				if (pf != NULL) {
					if (getLR(this)) pf->pl = tmp;
					else pf->pr = tmp;
				}
				pf = NULL;pl.reset();pr.reset();
			}
		}

	};

	//AVL��
	template<typename T>
	class AVLTreeNode :public std::enable_shared_from_this<AVLTreeNode<T> > {
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

		//���룬��Ϊ���ڵ���ܻ�䣬���Է��ظ��ڵ�
		AVLTNodePtr insert(AVLTNodePtr& ndptr) {
			assert(ndptr);
			AVLTNodePtr re;
			if (ndptr->obj < obj) {
				if (pl) { 
					pl->insert(ndptr);
					size_t Rhgt = pr ? pr->hgt : 0;
					if (2 == (pl->hgt - Rhgt)) {
						//��߱��ұ߸���2
						if (ndptr->obj < pl->obj) re = rotateL();
						else {
							pl->rotateR();
							re = rotateL();
						}
					}
				}
				else {
					setLChild(this, ndptr);
					ndptr->updateHeight();
				}
			}
			else {
				if (pr) {
					pr->insert(ndptr);
					size_t Lhgt = pl ? pl->hgt : 0;
					if (2 == (pr->hgt - Lhgt)) {
						//��߱��ұߵ���2
						if (ndptr->obj > pr->obj) re = rotateR();
						else {
							pr->rotateL();
							re = rotateR();
						}
					}
				}
				else {
					setRChild(this, ndptr);
					ndptr->updateHeight();
				}
			}
			if(re)	return re;
			return shared_from_this();
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
					pf = NULL; pl.reset();
				}
			}
			else if (!pl && pr) {
				//ֻ��������
				if (pf == NULL) breakRChild(this);
				else {
					pr->pf = pf;
					pf->pr = pr;
					pf = NULL; pr.reset();
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
				if (pf != NULL) {
					if (getLR(this)) pf->pl = tmp;
					else pf->pr = tmp;
				}
				pf = NULL; pl.reset(); pr.reset();
			}

		}

		//����ת��˳ʱ��
		AVLTNodePtr rotateL() {
			AVLTNodePtr re = pl;
			if (re->pr) re->pr->pf = this;
			pl = re->pr;
			if (pf == NULL) re->pr = shared_from_this();
			else {
				if (getLR(this)) {
					re->pr = pf->pl;
					pf->pl = re;
				}
				else {
					re->pr = pf->pr;
					pf->pr = re;
				}
			}
			re->pf = pf;
			pf = re.get();
			re->hgt = 0;//����߶�Ҫ���¼���
			updateHeight();
			return re;
		}
		//����ת����ʱ��
		AVLTNodePtr rotateR() {
			AVLTNodePtr re = pr;
			if (re->pl) re->pl->pf = this;
			pr = re->pl;
			if (pf == NULL) re->pl = shared_from_this();
			else {
				if (getLR(this)) {
					re->pl = pf->pl;
					pf->pl = re;
				}
				else {
					re->pl = pf->pr;
					pf->pr = re;
				}
			}
			re->pf = pf;
			pf = re.get();
			re->hgt = 0;
			updateHeight();
			return re;
		}

		//���ӽڵ����ϸ��¸߶�
		void updateHeight() {
			AVLTreeNode<T>* tmp = this;
			while (tmp != NULL) {
				size_t lh = (tmp->pl) ? tmp->pl->hgt : 0;
				size_t lr = (tmp->pr) ? tmp->pr->hgt : 0;
				size_t h = max(lh, lr) + 1;
				if (tmp->hgt == h && tmp != this) break;
				tmp->hgt = h;
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
		assert(nd);
		vec.push_back(nd);
		if (nd->pl) DLR(nd->pl, vec);
		if (nd->pr) DLR(nd->pr, vec);
	}
	//�������
	template<typename T>
	void LDR(std::shared_ptr<T>& nd, std::vector<std::shared_ptr<T> >& vec) {
		assert(nd);
		if (nd->pl) LDR(nd->pl, vec);
		vec.push_back(nd);
		if (nd->pr) LDR(nd->pr, vec);
	}
	//��������
	template<typename T>
	void LRD(std::shared_ptr<T>& nd, std::vector<std::shared_ptr<T> >& vec) {
		assert(nd);
		if (nd->pl) LRD(nd->pl, vec);
		if (nd->pr) LRD(nd->pr, vec);
		vec.push_back(nd);
	}

	//��ȡ��ȣ����ڵ����Ϊ0
	template<typename T>
	size_t getDepth(const T* pnode) {
		assert(pnode != NULL);
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
		assert(pnode != NULL);
		size_t lh = 0, rh = 0;
		if (pnode->pl) lh = getHeight(pnode->pl.get());
		if (pnode->pr) rh = getHeight(pnode->pr.get());
		return max(lh, rh) + 1;
	}
	//��ȡ���нڵ����
	template<typename T>
	size_t getNodeNum(const T* pnode) {
		assert(pnode != NULL);
		size_t num = 1;
		if (pnode->pl) num += getNodeNum(pnode->pl.get());
		if (pnode->pr) num += getNodeNum(pnode->pr.get());
		return num;
	}

	//��һ���ڵ���Ϊ��/���ӽڵ㣬��ԭ��/���ӽڵ�Ͽ�������Ľڵ�����ԭ���ڵ�Ͽ�
	template<typename T>
	void setLChild(T* pfather,std::shared_ptr<T>& pchild) {
		assert((pfather!=NULL) && pchild);
		pchild->pf = pfather;
		pfather->pl = pchild;
	}
	template<typename T>
	void setRChild(T* pfather, std::shared_ptr<T>& pchild) {
		assert((pfather != NULL) && pchild);
		pchild->pf = pfather;
		pfather->pr = pchild;
	}

	//����/�������Ͽ�
	template<typename T>
	void breakLChild(T* pnode) {
		assert((pnode != NULL) && pnode->pl);
		pnode->pl->pf = NULL;
		pnode->pl.reset();
	}
	template<typename T>
	void breakRChild(T* pnode) {
		assert((pnode != NULL) && pnode->pr);
		pnode->pr->pf = NULL;
		pnode->pr.reset();
	}

	//�ж��Ǹ��ڵ����ڵ㻹���ҽڵ㡣true��ʾ��ʹ��ǰӦ��鸸�ڵ��Ƿ�Ϊ��
	template<typename T>
	bool getLR(const T* pnode) {
		assert(pnode && pnode->pf != NULL);
		if (pnode == pnode->pf->pl.get()) return true;
		if (pnode == pnode->pf->pr.get()) return false;
		assert(0);//���Ǹ��ڵ�����ҽڵ㡣����
		return true;
	}

	//�ֲ����
	template<typename T>
	void traByLevel(std::shared_ptr<T>& nd, std::vector<std::shared_ptr<T> >& vec) {
		assert(nd);
		size_t pos1 = vec.size(),pos2;
		vec.push_back(nd);
		while (pos1 < vec.size()) {
			pos2 = vec.size();
			while (pos1 < pos2) {
				if (vec[pos1]->pl) vec.push_back(vec[pos1]->pl);
				if (vec[pos1]->pr) vec.push_back(vec[pos1]->pr);
				++pos1;
			}
		}
	}
}


