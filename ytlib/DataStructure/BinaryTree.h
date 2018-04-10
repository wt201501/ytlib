#pragma once
#include <ytlib/Common/Util.h>
#include <memory>
#include <vector>


namespace ytlib {
	//�����������ʵ��---------------------------------------------------------------------------
	//ȫ��ʹ��ģ�廯�����ﲻ��ʹ�ü̳У���Ϊ�г�Ա���������͵�����ָ��
	//һ��BinTreeNodeʵ����ʾһ���������ڵ㣬Ҳ��ʾ�Դ˽ڵ�Ϊ���ڵ��һ�ö�����
	//������ڵ㱻��������ô�������������ӽڵ㽫�������������ӽڵ������������ָ��ָ��
	template<typename T>
	class BinTreeNode {
	private:
		typedef std::shared_ptr<BinTreeNode<T> > nodePtr;
	public:
		BinTreeNode():pf(NULL){}
		explicit BinTreeNode(const T& _obj) :obj(_obj), pf(NULL) {}

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
		explicit BinSearchTreeNode(const T& _obj) :obj(_obj), pf(NULL) {}

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

		//ɾ����ǰ�ڵ㣬����������Ľڵ�
		BSTNodePtr erase() {
			if (!pl && !pr) {
				//���Ҷ�Ϊ�գ�ΪҶ�ӽڵ�
				if (pf != NULL) {
					if (getLR(this)) breakLChild(pf);
					else breakRChild(pf);
				}
				return BSTNodePtr();
			}
			if (pl && !pr) {
				//ֻ��������
				BSTNodePtr re = pl;
				if (pf == NULL) breakLChild(this);
				else {
					pl->pf = pf;
					pf->pl = pl;
					pf = NULL;pl.reset();
				}
				return re;
			}
			if (!pl && pr) {
				//ֻ��������
				BSTNodePtr re = pr;
				if (pf == NULL) breakRChild(this);
				else {
					pr->pf = pf;
					pf->pr = pr;
					pf = NULL;pr.reset();
				}
				return re;
			}
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
			return tmp;
		}
	};

	//AVL��
	template<typename T>
	class AVLTreeNode :public std::enable_shared_from_this<AVLTreeNode<T> > {
	private:
		typedef std::shared_ptr<AVLTreeNode<T> > AVLTNodePtr;
	public:
		AVLTreeNode() :pf(NULL), hgt(1){}
		explicit AVLTreeNode(const T& _obj) :obj(_obj), pf(NULL), hgt(1) {}

		T obj;
		AVLTreeNode<T>* pf;//���ڵ�
		AVLTNodePtr pl;//���ӽڵ�
		AVLTNodePtr pr;//���ӽڵ�
		size_t hgt;//�ڵ�߶�

#define HGT(p)	((p)?p->hgt:0)

		//���룬��Ϊ���ڵ���ܻ�䣬���Է��ظ��ڵ�
		AVLTNodePtr insert(AVLTNodePtr& ndptr) {
			assert(ndptr);
			//�ҵ�����Ҫ����ĵط��ĸ��ڵ�
			AVLTreeNode<T>* pos = this, *tmppos = (ndptr->obj < obj) ? pl.get() : pr.get();
			while (tmppos != NULL) {
				pos = tmppos;
				tmppos = (ndptr->obj < pos->obj) ? pos->pl.get() : pos->pr.get();
			}
			if (ndptr->obj == pos->obj) return shared_from_this();//�������ظ�
			if (ndptr->obj < pos->obj) setLChild(pos, ndptr);
			else setRChild(pos, ndptr);

			ndptr->hgt = 1;
			//���¸߶ȣ�������ת
			AVLTNodePtr re;
			AVLTreeNode<T>* end = pf;
			while (pos != end) {
				re.reset();
				size_t curhgt = pos->hgt;
				size_t lh = HGT(pos->pl), lr = HGT(pos->pr);
				if (lh >= lr + 2) {
					//��߱��ұ߸���2
					if (HGT(pos->pl->pl) >= HGT(pos->pl->pr)) re = pos->rotateL();
					else {
						pos->pl->rotateR();
						re = pos->rotateL();
					}
				}
				else if (lr >= lh + 2) {
					//�ұ߱���߸���2
					if (HGT(pos->pr->pr) >= HGT(pos->pr->pl)) re = pos->rotateR();
					else {
						pos->pr->rotateL();
						re = pos->rotateR();
					}
				}
				//���������ת�ˣ�˵��֮ǰ���Ҹ߶����2��˵���ýڵ�߶�һ�������ı�
				size_t cghgt;
				if (re) {
					cghgt = re->hgt;
					pos = re->pf;
				}
				else {
					cghgt = pos->hgt = max(lh, lr) + 1;
					if (curhgt == cghgt) return shared_from_this();
					pos = pos->pf;
				}
				
			}
			if (re) return re;
			return shared_from_this();
		}

		//�ڵ�ǰ�ڵ�Ϊ���ڵ������ɾ��һ���ڵ㣬������ɾ����ĸ��ڵ�
		AVLTNodePtr erase(AVLTNodePtr& ndptr) {
			if (!ndptr) return shared_from_this();
			//��ȷ��Ҫɾ���Ľڵ����Լ����ӽڵ�
			AVLTreeNode<T>* pos = ndptr.get();
			while (pos != NULL) {
				if (pos == this) break;
				pos = pos->pf;
			}
			if (pos == this) return _erase(ndptr);
			return shared_from_this();
		}

		AVLTNodePtr erase(const T& val) {
			return _erase(binSearch<AVLTreeNode<T>, T>(shared_from_this(), val));
		}

	private:
		//�������ظ��ģ�ɾ����һ���ҵ���
		AVLTNodePtr _erase(AVLTNodePtr& ndptr) {
			assert(pf == NULL);//������Ҫ�Ǹ��ڵ�
			if (!ndptr) return shared_from_this();
			AVLTNodePtr proot = shared_from_this();//���Ҫɾ��������������Ҫһ��ָ��������root�ڵ�
			AVLTreeNode<T>* pos = ndptr->pf;
			if (!(ndptr->pl) && !(ndptr->pr)) {
				//���Ҷ�Ϊ�գ�ΪҶ�ӽڵ�
				if (ndptr->pf != NULL) {
					if (getLR(ndptr.get())) breakLChild(ndptr->pf);
					else breakRChild(ndptr->pf);
				}
				else {
					//��������һ��Ҫɾ���ĸ��ڵ�
					return AVLTNodePtr();
				}
			}
			else if (ndptr->pl && !(ndptr->pr)) {
				//ֻ��������
				if (ndptr->pf == NULL) {
					proot = ndptr->pl;
					breakLChild(ndptr.get());
				}
				else {
					ndptr->pl->pf = ndptr->pf;
					ndptr->pf->pl = ndptr->pl;
					ndptr->pf = NULL; ndptr->pl.reset();
				}
			}
			else if (!(ndptr->pl) && ndptr->pr) {
				//ֻ��������
				if (ndptr->pf == NULL) { 
					proot = ndptr->pr;
					breakRChild(ndptr.get());
				}
				else {
					ndptr->pr->pf = ndptr->pf;
					ndptr->pf->pr = ndptr->pr;
					ndptr->pf = NULL; ndptr->pr.reset();
				}
			}
			else {
				//����������ǰ��
				AVLTNodePtr tmp = ndptr->pl;
				if (tmp->pr) {
					//���ӽڵ������������ҵ���ǰ��
					while (tmp->pr) tmp = tmp->pr;
					tmp->pf->pr = tmp->pl;
					if (tmp->pl) tmp->pl->pf = tmp->pf;
					tmp->pl = ndptr->pl; ndptr->pl->pf = tmp.get();
					pos = tmp->pf;
				}
				else pos = tmp.get();
				tmp->pf = ndptr->pf;
				tmp->pr = ndptr->pr; ndptr->pr->pf = tmp.get();
				if (ndptr->pf != NULL) {
					if (getLR(ndptr.get())) ndptr->pf->pl = tmp;
					else ndptr->pf->pr = tmp;
				}
				else proot = tmp;
				ndptr->pf = NULL; ndptr->pl.reset(); ndptr->pr.reset();
				tmp->hgt = ndptr->hgt;
			}
			//���¸߶ȣ�������ת
			AVLTNodePtr re;
			while (pos != NULL) {
				re.reset();
				size_t curhgt = pos->hgt;
				size_t lh = HGT(pos->pl), lr = HGT(pos->pr);
				if (lh >= lr + 2) {
					//��߱��ұ߸���2
					if (HGT(pos->pl->pl) >= HGT(pos->pl->pr)) re = pos->rotateL();
					else {
						pos->pl->rotateR();
						re = pos->rotateL();
					}
				}
				else if (lr >= lh + 2) {
					//�ұ߱���߸���2
					if (HGT(pos->pr->pr) >= HGT(pos->pr->pl)) re = pos->rotateR();
					else {
						pos->pr->rotateL();
						re = pos->rotateR();
					}
				}
				//���������ת�ˣ�˵��֮ǰ���Ҹ߶����2��˵���ýڵ�߶�һ�������ı�
				size_t cghgt;
				if (re) {
					cghgt = re->hgt;
					pos = re->pf;
				}
				else {
					cghgt = pos->hgt = max(lh, lr) + 1;
					if (curhgt == cghgt) break;
					pos = pos->pf;
				}
			}
			if (re) return re;
			return proot;
		}

		inline size_t getHgt() {
			size_t lh = (pl) ? pl->hgt : 0;
			size_t lr = (pr) ? pr->hgt : 0;
			return max(lh, lr) + 1;
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
			hgt = getHgt();
			re->hgt = re->getHgt();
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
			hgt = getHgt();
			re->hgt = re->getHgt();
			return re;
		}
	};

	//�������todo������
	template<typename T>
	class BRTreeNode :public std::enable_shared_from_this<BRTreeNode<T> > {
	private:
		typedef std::shared_ptr<BRTreeNode<T> > BRTreeNodePtr;
	public:
		BRTreeNode() :pf(NULL), color(false){}
		explicit BRTreeNode(const T& _obj) :obj(_obj), pf(NULL), color(false) {}

		T obj;
		BRTreeNode<T>* pf;//���ڵ�
		BRTreeNodePtr pl;//���ӽڵ�
		BRTreeNodePtr pr;//���ӽڵ�
		bool color;//��ɫ��trueΪ�죬falseΪ��

		//���룬��Ϊ���ڵ���ܻ�䣬���Է��ظ��ڵ�
		BRTreeNodePtr insert(BRTreeNodePtr& ndptr) {
			assert(ndptr && !color);
			//�ҵ�����Ҫ����ĵط��ĸ��ڵ�
			BRTreeNode<T>* pos = this, *tmppos = (ndptr->obj < obj) ? pl.get() : pr.get();
			while (tmppos != NULL) {
				pos = tmppos;
				tmppos = (ndptr->obj < pos->obj) ? pos->pl.get() : pos->pr.get();
			}
			if (ndptr->obj == pos->obj) return shared_from_this();//�������ظ�
			if (ndptr->obj < pos->obj) setLChild(pos, ndptr);
			else setRChild(pos, ndptr);

			//����ڵ����ɫ���Ǻ�ɫ
			ndptr->color = true;
			
			BRTreeNodePtr re;
			BRTreeNode<T>* end = pf;
			tmppos = ndptr.get();
			while (pos != end) {
				re.reset();
				//���ڵ��Ǻ�ɫ
				if (!(pos->color)) {
					return shared_from_this();
				}
				BRTreeNode<T>* uncle = (getLR(pos) ? pos->pf->pr.get() : pos->pf->pl.get());
				if (uncle!=NULL && uncle->color) {
					//����ڵ�ĸ��ڵ��������ڵ��Ϊ��ɫ��
					pos->color = uncle->color = false;
					pos->pf->color = true;
					tmppos = pos->pf;
					pos = tmppos->pf;
				}
				else {
					//����ڵ�ĸ��ڵ��Ǻ�ɫ������ڵ��Ǻ�ɫ
					if (getLR(pos)) {
						//���ڵ����游�ڵ����֧
						if (getLR(tmppos)) {
							//����ڵ����丸�ڵ�����ӽڵ�
							pos->color = false;
							pos->pf->color = true;
							re = pos->pf->rotateL();
							break;
						}
						else {
							//����ڵ����丸�ڵ�����ӽڵ�
							pos->rotateR();
							tmppos = pos;
							pos = tmppos->pf;
						}
					}
					else {
						//���ڵ����游�ڵ����֧
						if (getLR(tmppos)) {
							//����ڵ����丸�ڵ�����ӽڵ�
							pos->rotateL();
							tmppos = pos;
							pos = tmppos->pf;
						}
						else {
							//����ڵ����丸�ڵ�����ӽڵ�
							pos->color = false;
							pos->pf->color = true;
							re = pos->pf->rotateR();
							break;
						}
					}
		
				}
			}
			if (pos == end) tmppos->color = false;
			if (re && (re->pf == end)) return re;
			return shared_from_this();
		}

		//�ڵ�ǰ�ڵ�Ϊ���ڵ������ɾ��һ���ڵ㣬������ɾ����ĸ��ڵ�
		BRTreeNodePtr erase(BRTreeNodePtr& ndptr) {
			if (!ndptr) return shared_from_this();
			//��ȷ��Ҫɾ���Ľڵ����Լ����ӽڵ�
			BRTreeNode<T>* pos = ndptr.get();
			while (pos != NULL) {
				if (pos == this) break;
				pos = pos->pf;
			}
			if (pos == this) return _erase(ndptr);
			return shared_from_this();
		}

		BRTreeNodePtr erase(const T& val) {
			return _erase(binSearch<BRTreeNode<T>, T>(shared_from_this(), val));
		}
	private:
		BRTreeNodePtr _erase(BRTreeNodePtr& ndptr) {
			assert(pf == NULL);//������Ҫ�Ǹ��ڵ�
			if (!ndptr) return shared_from_this();
			BRTreeNodePtr proot = shared_from_this();//���Ҫɾ��������������Ҫһ��ָ��������root�ڵ�
			BRTreeNode<T>* pos = ndptr->pf;
			if (!(ndptr->pl) && !(ndptr->pr)) {
				//���Ҷ�Ϊ�գ�ΪҶ�ӽڵ�
				if (ndptr->pf != NULL) {
					if (getLR(ndptr.get())) breakLChild(ndptr->pf);
					else breakRChild(ndptr->pf);
				}
				else {
					//��������һ��Ҫɾ���ĸ��ڵ�
					return BRTreeNodePtr();
				}
			}
			else if (ndptr->pl && !(ndptr->pr)) {
				//ֻ��������
				if (ndptr->pf == NULL) {
					proot = ndptr->pl;
					breakLChild(ndptr.get());
				}
				else {
					ndptr->pl->pf = ndptr->pf;
					ndptr->pf->pl = ndptr->pl;
					ndptr->pf = NULL; ndptr->pl.reset();
				}
			}
			else if (!(ndptr->pl) && ndptr->pr) {
				//ֻ��������
				if (ndptr->pf == NULL) {
					proot = ndptr->pr;
					breakRChild(ndptr.get());
				}
				else {
					ndptr->pr->pf = ndptr->pf;
					ndptr->pf->pr = ndptr->pr;
					ndptr->pf = NULL; ndptr->pr.reset();
				}
			}
			else {
				//����������ǰ��
				BRTreeNodePtr tmp = ndptr->pl;
				if (tmp->pr) {
					//���ӽڵ������������ҵ���ǰ��
					while (tmp->pr) tmp = tmp->pr;
					tmp->pf->pr = tmp->pl;
					if (tmp->pl) tmp->pl->pf = tmp->pf;
					tmp->pl = ndptr->pl; ndptr->pl->pf = tmp.get();
					pos = tmp->pf;
				}
				else pos = tmp.get();
				tmp->pf = ndptr->pf;
				tmp->pr = ndptr->pr; ndptr->pr->pf = tmp.get();
				if (ndptr->pf != NULL) {
					if (getLR(ndptr.get())) ndptr->pf->pl = tmp;
					else ndptr->pf->pr = tmp;
				}
				else proot = tmp;
				ndptr->pf = NULL; ndptr->pl.reset(); ndptr->pr.reset();
				tmp->hgt = ndptr->hgt;
			}




		}
		//����ת��˳ʱ��
		BRTreeNodePtr rotateL() {
			BRTreeNodePtr re = pl;
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
			return re;
		}
		//����ת����ʱ��
		BRTreeNodePtr rotateR() {
			BRTreeNodePtr re = pr;
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
			return re;
		}

	};

	//һЩ���õ��㷨---------------------------------------------------------------------------

	//�ڶ����������н��в���
	template<typename NodeType, typename ValType>
	std::shared_ptr<NodeType> binSearch(const std::shared_ptr<NodeType>& proot, const ValType& val) {
		std::shared_ptr<NodeType> p = proot;
		while (p) {
			if (p->obj == val) return p;
			if (val < p->obj) p = p->pl;
			else if(val > p->obj) p = p->pr;
			else return std::shared_ptr<NodeType>();
		}
		return std::shared_ptr<NodeType>();
	}

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

	//�����������л�
	template<typename T>
	void SerializeTree(const std::shared_ptr<T>& proot, std::vector<std::shared_ptr<T> >& vec) {
		vec.push_back(proot);
		if (!proot)	return;
		SerializeTree(proot->pl, vec);
		SerializeTree(proot->pr, vec);
	}
	//�����л�
	template<typename T>
	void DeserializeTree(std::shared_ptr<T>& proot, typename std::vector<std::shared_ptr<T> >::iterator& itr) {
		if (*itr) {
			proot = *itr;
			DeserializeTree(proot->pl, ++itr);
			DeserializeTree(proot->pr, ++itr);
		}
	}
	//���ĸ���
	template<typename T>
	std::shared_ptr<T> copyTree(const std::shared_ptr<T>& proot) {
		std::shared_ptr<T> p = std::make_shared<T>(*proot);
		if (proot->pl) setLChild(p.get(), copyTree(proot->pl));
		if (proot->pr) setRChild(p.get(), copyTree(proot->pr));
		return p;
	}


}


