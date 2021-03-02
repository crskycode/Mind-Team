#ifndef UTLRBTREE_H
#define UTLRBTREE_H

#include "UtlMemory.h"

template <typename T >
class CDefOps
{
public:
	static bool LessFunc(const T &lhs, const T &rhs)	{ return (lhs < rhs); }
};

#define DefLessFunc( type ) CDefOps<type>::LessFunc

inline bool StringLessThan(const char * const &lhs, const char * const &rhs)			{ return (strcmp(lhs, rhs) < 0); }
inline bool CaselessStringLessThan(const char * const &lhs, const char * const &rhs)	{ return (stricmp(lhs, rhs) < 0); }

template <> inline bool CDefOps<const char *>::LessFunc(const char * const &lhs, const char * const &rhs)	{ return StringLessThan(lhs, rhs); }
template <> inline bool CDefOps<char *>::LessFunc(char * const &lhs, char * const &rhs)						{ return StringLessThan(lhs, rhs); }

template <typename RBTREE_T>
void SetDefLessFunc(RBTREE_T &RBTree)
{
#ifdef _WIN32
	RBTree.SetLessFunc(DefLessFunc(RBTREE_T::KeyType_t));
#elif _LINUX
	RBTree.SetLessFunc(DefLessFunc(typename RBTREE_T::KeyType_t));
#endif
}

template <class T, class I = unsigned short>
class CUtlRBTree
{
public:
	typedef bool(*LessFunc_t)(T const &, T const &);

	typedef T KeyType_t;
	typedef T ElemType_t;
	typedef I IndexType_t;

	CUtlRBTree(int growSize = 0, int initSize = 0, LessFunc_t lessfunc = 0);
	~CUtlRBTree();

	T&			Element(I i);
	T const		&Element(I i) const;
	T&			operator[](I i);
	T const		&operator[](I i) const;

	I  Root() const;
	unsigned int Count() const;
	I  MaxElement() const;
	I  Parent(I i) const;
	I  LeftChild(I i) const;
	I  RightChild(I i) const;
	bool  IsLeftChild(I i) const;
	bool  IsRightChild(I i) const;
	bool  IsRoot(I i) const;
	bool  IsLeaf(I i) const;
	bool  IsValidIndex(I i) const;
	bool  IsValid() const;
	static I InvalidIndex();
	int   Depth(I node) const;
	int   Depth() const;
	void SetLessFunc(LessFunc_t func);
	I  NewNode();
	I  Insert(T const &insert);
	void Insert(const T *pArray, int nItems);
	I  Find(T const &search) const;
	void     RemoveAt(I i);
	bool     Remove(T const &remove);
	void     RemoveAll();
	void  FreeNode(I i);
	I  FirstInorder() const;
	I  NextInorder(I i) const;
	I  PrevInorder(I i) const;
	I  LastInorder() const;
	I  FirstPreorder() const;
	I  NextPreorder(I i) const;
	I  PrevPreorder(I i) const;
	I  LastPreorder() const;
	I  FirstPostorder() const;
	I  NextPostorder(I i) const;
	void	Reinsert(I elem);

protected:
	enum NodeColor_t
	{
		RED = 0,
		BLACK
	};

	struct Links_t
	{
		I  m_Left;
		I  m_Right;
		I  m_Parent;
		I  m_Tag;
	};

	struct Node_t : public Links_t
	{
		T  m_Data;
	};

	void  SetParent(I i, I parent);
	void  SetLeftChild(I i, I child);
	void  SetRightChild(I i, I child);
	void  LinkToParent(I i, I parent, bool isLeft);

	Links_t const	&Links(I i) const;
	Links_t			&Links(I i);

	bool IsRed(I i) const;
	bool IsBlack(I i) const;

	NodeColor_t Color(I i) const;
	void        SetColor(I i, NodeColor_t c);

	void RotateLeft(I i);
	void RotateRight(I i);
	void InsertRebalance(I i);
	void RemoveRebalance(I i);

	I  InsertAt(I parent, bool leftchild);

	CUtlRBTree(CUtlRBTree<T, I> const &tree);

	void FindInsertionPosition(T const &insert, I &parent, bool &leftchild);

	void	Unlink(I elem);
	void	Link(I elem);

	LessFunc_t m_LessFunc;

	CUtlMemory<Node_t> m_Elements;
	I m_Root;
	I m_NumElements;
	I m_FirstFree;
	I m_TotalElements;

	Node_t*   m_pElements;

	void ResetDbgInfo()
	{
		m_pElements = (Node_t*)m_Elements.Base();
	}
};


template <class T, class I>
CUtlRBTree<T, I>::CUtlRBTree(int growSize, int initSize, LessFunc_t lessfunc) :
m_Elements(growSize, initSize),
m_LessFunc(lessfunc),
m_Root(InvalidIndex()),
m_NumElements(0), m_TotalElements(0),
m_FirstFree(InvalidIndex())
{
	ResetDbgInfo();
}

template <class T, class I>
CUtlRBTree<T, I>::~CUtlRBTree()
{
}

template <class T, class I>
inline T &CUtlRBTree<T, I>::Element(I i)
{
	return m_Elements[i].m_Data;
}

template <class T, class I>
inline T const &CUtlRBTree<T, I>::Element(I i) const
{
	return m_Elements[i].m_Data;
}

template <class T, class I>
inline T &CUtlRBTree<T, I>::operator[](I i)
{
	return Element(i);
}

template <class T, class I>
inline T const &CUtlRBTree<T, I>::operator[](I i) const
{
	return Element(i);
}

template <class T, class I>
inline	I  CUtlRBTree<T, I>::Root() const
{
	return m_Root;
}

template <class T, class I>
inline	unsigned int CUtlRBTree<T, I>::Count() const
{
	return (unsigned int)m_NumElements;
}

template <class T, class I>
inline	I  CUtlRBTree<T, I>::MaxElement() const
{
	return (I)m_TotalElements;
}

template <class T, class I>
inline	I CUtlRBTree<T, I>::Parent(I i) const
{
	return Links(i).m_Parent;
}

template <class T, class I>
inline	I CUtlRBTree<T, I>::LeftChild(I i) const
{
	return Links(i).m_Left;
}

template <class T, class I>
inline	I CUtlRBTree<T, I>::RightChild(I i) const
{
	return Links(i).m_Right;
}

template <class T, class I>
inline	bool CUtlRBTree<T, I>::IsLeftChild(I i) const
{
	return LeftChild(Parent(i)) == i;
}

template <class T, class I>
inline	bool CUtlRBTree<T, I>::IsRightChild(I i) const
{
	return RightChild(Parent(i)) == i;
}

template <class T, class I>
inline	bool CUtlRBTree<T, I>::IsRoot(I i) const
{
	return i == m_Root;
}

template <class T, class I>
inline	bool CUtlRBTree<T, I>::IsLeaf(I i) const
{
	return (LeftChild(i) == InvalidIndex()) && (RightChild(i) == InvalidIndex());
}

template <class T, class I>
inline	bool CUtlRBTree<T, I>::IsValidIndex(I i) const
{
	return LeftChild(i) != i;
}

template <class T, class I>
I CUtlRBTree<T, I>::InvalidIndex()
{
	return (I)~0;
}

template <class T, class I>
inline int CUtlRBTree<T, I>::Depth() const
{
	return Depth(Root());
}

template <class T, class I>
inline void  CUtlRBTree<T, I>::SetParent(I i, I parent)
{
	Links(i).m_Parent = parent;
}

template <class T, class I>
inline void  CUtlRBTree<T, I>::SetLeftChild(I i, I child)
{
	Links(i).m_Left = child;
}

template <class T, class I>
inline void  CUtlRBTree<T, I>::SetRightChild(I i, I child)
{
	Links(i).m_Right = child;
}

template <class T, class I>
inline typename CUtlRBTree<T, I>::Links_t const &CUtlRBTree<T, I>::Links(I i) const
{
	static Links_t s_Sentinel =
	{
		InvalidIndex(), InvalidIndex(), InvalidIndex(), CUtlRBTree<T, I>::BLACK
	};

	return (i != InvalidIndex()) ? *(Links_t*)&m_Elements[i] :
		*(Links_t*)&s_Sentinel;
}

template <class T, class I>
inline typename CUtlRBTree<T, I>::Links_t &CUtlRBTree<T, I>::Links(I i)
{
	return *(Links_t *)&m_Elements[i];
}

template <class T, class I>
inline bool CUtlRBTree<T, I>::IsRed(I i) const
{
	return (Links(i).m_Tag == RED);
}

template <class T, class I>
inline bool CUtlRBTree<T, I>::IsBlack(I i) const
{
	return (Links(i).m_Tag == BLACK);
}


template <class T, class I>
inline typename CUtlRBTree<T, I>::NodeColor_t  CUtlRBTree<T, I>::Color(I i) const
{
	return (NodeColor_t)Links(i).m_Tag;
}

template <class T, class I>
inline void CUtlRBTree<T, I>::SetColor(I i, typename CUtlRBTree<T, I>::NodeColor_t c)
{
	Links(i).m_Tag = (I)c;
}

template <class T, class I>
I  CUtlRBTree<T, I>::NewNode()
{
	I newElem;

	if (m_FirstFree == InvalidIndex())
	{
		if (m_Elements.NumAllocated() == m_TotalElements)
			m_Elements.Grow();
		newElem = m_TotalElements++;
	}
	else
	{
		newElem = m_FirstFree;
		m_FirstFree = RightChild(m_FirstFree);
	}

#ifdef _DEBUG
	Links_t &node = Links(newElem);
	node.m_Left = node.m_Right = node.m_Parent = InvalidIndex();
#endif

	Construct(&Element(newElem));
	ResetDbgInfo();

	return newElem;
}

template <class T, class I>
void  CUtlRBTree<T, I>::FreeNode(I i)
{
	assert(IsValidIndex(i) && (i != InvalidIndex()));
	Destruct(&Element(i));
	SetLeftChild(i, i);
	SetRightChild(i, m_FirstFree);
	m_FirstFree = i;
}

template <class T, class I>
void CUtlRBTree<T, I>::RotateLeft(I elem)
{
	I rightchild = RightChild(elem);
	SetRightChild(elem, LeftChild(rightchild));
	if (LeftChild(rightchild) != InvalidIndex())
		SetParent(LeftChild(rightchild), elem);

	if (rightchild != InvalidIndex())
		SetParent(rightchild, Parent(elem));
	if (!IsRoot(elem))
	{
		if (IsLeftChild(elem))
			SetLeftChild(Parent(elem), rightchild);
		else
			SetRightChild(Parent(elem), rightchild);
	}
	else
		m_Root = rightchild;

	SetLeftChild(rightchild, elem);
	if (elem != InvalidIndex())
		SetParent(elem, rightchild);
}

template <class T, class I>
void CUtlRBTree<T, I>::RotateRight(I elem)
{
	I leftchild = LeftChild(elem);
	SetLeftChild(elem, RightChild(leftchild));
	if (RightChild(leftchild) != InvalidIndex())
		SetParent(RightChild(leftchild), elem);

	if (leftchild != InvalidIndex())
		SetParent(leftchild, Parent(elem));
	if (!IsRoot(elem))
	{
		if (IsRightChild(elem))
			SetRightChild(Parent(elem), leftchild);
		else
			SetLeftChild(Parent(elem), leftchild);
	}
	else
		m_Root = leftchild;

	SetRightChild(leftchild, elem);
	if (elem != InvalidIndex())
		SetParent(elem, leftchild);
}

template <class T, class I>
void CUtlRBTree<T, I>::InsertRebalance(I elem)
{
	while (!IsRoot(elem) && (Color(Parent(elem)) == RED))
	{
		I parent = Parent(elem);
		I grandparent = Parent(parent);

		if (IsLeftChild(parent))
		{
			I uncle = RightChild(grandparent);
			if (IsRed(uncle))
			{
				SetColor(parent, BLACK);
				SetColor(uncle, BLACK);
				SetColor(grandparent, RED);
				elem = grandparent;
			}
			else
			{
				if (IsRightChild(elem))
				{
					elem = parent;
					RotateLeft(elem);
					parent = Parent(elem);
					grandparent = Parent(parent);
				}
				SetColor(parent, BLACK);
				SetColor(grandparent, RED);
				RotateRight(grandparent);
			}
		}
		else
		{
			I uncle = LeftChild(grandparent);
			if (IsRed(uncle))
			{
				SetColor(parent, BLACK);
				SetColor(uncle, BLACK);
				SetColor(grandparent, RED);
				elem = grandparent;
			}
			else
			{
				if (IsLeftChild(elem))
				{
					elem = parent;
					RotateRight(parent);
					parent = Parent(elem);
					grandparent = Parent(parent);
				}
				SetColor(parent, BLACK);
				SetColor(grandparent, RED);
				RotateLeft(grandparent);
			}
		}
	}
	SetColor(m_Root, BLACK);
}

template <class T, class I>
I CUtlRBTree<T, I>::InsertAt(I parent, bool leftchild)
{
	I i = NewNode();
	LinkToParent(i, parent, leftchild);
	++m_NumElements;
	return i;
}

template <class T, class I>
void CUtlRBTree<T, I>::LinkToParent(I i, I parent, bool isLeft)
{
	Links_t &elem = Links(i);
	elem.m_Parent = parent;
	elem.m_Left = elem.m_Right = InvalidIndex();
	elem.m_Tag = RED;

	if (parent != InvalidIndex())
	{
		if (isLeft)
			Links(parent).m_Left = i;
		else
			Links(parent).m_Right = i;
	}
	else
	{
		m_Root = i;
	}

	InsertRebalance(i);
}

template <class T, class I>
void CUtlRBTree<T, I>::RemoveRebalance(I elem)
{
	while (elem != m_Root && IsBlack(elem))
	{
		I parent = Parent(elem);

		if (elem == LeftChild(parent))
		{
			I sibling = RightChild(parent);
			if (IsRed(sibling))
			{
				SetColor(sibling, BLACK);
				SetColor(parent, RED);
				RotateLeft(parent);

				parent = Parent(elem);
				sibling = RightChild(parent);
			}
			if ((IsBlack(LeftChild(sibling))) && (IsBlack(RightChild(sibling))))
			{
				if (sibling != InvalidIndex())
					SetColor(sibling, RED);
				elem = parent;
			}
			else
			{
				if (IsBlack(RightChild(sibling)))
				{
					SetColor(LeftChild(sibling), BLACK);
					SetColor(sibling, RED);
					RotateRight(sibling);

					parent = Parent(elem);
					sibling = RightChild(parent);
				}
				SetColor(sibling, Color(parent));
				SetColor(parent, BLACK);
				SetColor(RightChild(sibling), BLACK);
				RotateLeft(parent);
				elem = m_Root;
			}
		}
		else
		{
			I sibling = LeftChild(parent);
			if (IsRed(sibling))
			{
				SetColor(sibling, BLACK);
				SetColor(parent, RED);
				RotateRight(parent);

				parent = Parent(elem);
				sibling = LeftChild(parent);
			}
			if ((IsBlack(RightChild(sibling))) && (IsBlack(LeftChild(sibling))))
			{
				if (sibling != InvalidIndex())
					SetColor(sibling, RED);
				elem = parent;
			}
			else
			{
				if (IsBlack(LeftChild(sibling)))
				{
					SetColor(RightChild(sibling), BLACK);
					SetColor(sibling, RED);
					RotateLeft(sibling);

					parent = Parent(elem);
					sibling = LeftChild(parent);
				}
				SetColor(sibling, Color(parent));
				SetColor(parent, BLACK);
				SetColor(LeftChild(sibling), BLACK);
				RotateRight(parent);
				elem = m_Root;
			}
		}
	}
	SetColor(elem, BLACK);
}

template <class T, class I>
void CUtlRBTree<T, I>::Unlink(I elem)
{
	if (elem != InvalidIndex())
	{
		I x, y;

		if ((LeftChild(elem) == InvalidIndex()) ||
			(RightChild(elem) == InvalidIndex()))
		{
			y = elem;
		}
		else
		{
			y = RightChild(elem);
			while (LeftChild(y) != InvalidIndex())
				y = LeftChild(y);
		}

		if (LeftChild(y) != InvalidIndex())
			x = LeftChild(y);
		else
			x = RightChild(y);

		if (x != InvalidIndex())
			SetParent(x, Parent(y));
		if (!IsRoot(y))
		{
			if (IsLeftChild(y))
				SetLeftChild(Parent(y), x);
			else
				SetRightChild(Parent(y), x);
		}
		else
			m_Root = x;

		NodeColor_t ycolor = Color(y);
		if (y != elem)
		{
			SetParent(y, Parent(elem));
			SetRightChild(y, RightChild(elem));
			SetLeftChild(y, LeftChild(elem));

			if (!IsRoot(elem))
			if (IsLeftChild(elem))
				SetLeftChild(Parent(elem), y);
			else
				SetRightChild(Parent(elem), y);
			else
				m_Root = y;

			if (LeftChild(y) != InvalidIndex())
				SetParent(LeftChild(y), y);
			if (RightChild(y) != InvalidIndex())
				SetParent(RightChild(y), y);

			SetColor(y, Color(elem));
		}

		if ((x != InvalidIndex()) && (ycolor == BLACK))
			RemoveRebalance(x);
	}
}

template <class T, class I>
void CUtlRBTree<T, I>::Link(I elem)
{
	if (elem != InvalidIndex())
	{
		I parent;
		bool leftchild;

		FindInsertionPosition(Element(elem), parent, leftchild);

		LinkToParent(elem, parent, leftchild);
	}
}

template <class T, class I>
void CUtlRBTree<T, I>::RemoveAt(I elem)
{
	if (elem != InvalidIndex())
	{
		Unlink(elem);

		FreeNode(elem);
		--m_NumElements;
	}
}

template <class T, class I> bool CUtlRBTree<T, I>::Remove(T const &search)
{
	I node = Find(search);
	if (node != InvalidIndex())
	{
		RemoveAt(node);
		return true;
	}
	return false;
}

template <class T, class I>
void CUtlRBTree<T, I>::RemoveAll()
{
	I prev = InvalidIndex();
	for (int i = (int)m_TotalElements; --i >= 0;)
	{
		I idx = (I)i;
		if (IsValidIndex(idx))
			Destruct(&Element(idx));
		SetRightChild(idx, prev);
		SetLeftChild(idx, idx);
		prev = idx;
	}
	m_FirstFree = m_TotalElements ? (I)0 : InvalidIndex();
	m_Root = InvalidIndex();
	m_NumElements = 0;
}

template <class T, class I>
I CUtlRBTree<T, I>::FirstInorder() const
{
	I i = m_Root;
	while (LeftChild(i) != InvalidIndex())
		i = LeftChild(i);
	return i;
}

template <class T, class I>
I CUtlRBTree<T, I>::NextInorder(I i) const
{
	assert(IsValidIndex(i));

	if (RightChild(i) != InvalidIndex())
	{
		i = RightChild(i);
		while (LeftChild(i) != InvalidIndex())
			i = LeftChild(i);
		return i;
	}

	I parent = Parent(i);
	while (IsRightChild(i))
	{
		i = parent;
		if (i == InvalidIndex()) break;
		parent = Parent(i);
	}
	return parent;
}

template <class T, class I>
I CUtlRBTree<T, I>::PrevInorder(I i) const
{
	assert(IsValidIndex(i));

	if (LeftChild(i) != InvalidIndex())
	{
		i = LeftChild(i);
		while (RightChild(i) != InvalidIndex())
			i = RightChild(i);
		return i;
	}

	I parent = Parent(i);
	while (IsLeftChild(i))
	{
		i = parent;
		if (i == InvalidIndex()) break;
		parent = Parent(i);
	}
	return parent;
}

template <class T, class I>
I CUtlRBTree<T, I>::LastInorder() const
{
	I i = m_Root;
	while (RightChild(i) != InvalidIndex())
		i = RightChild(i);
	return i;
}

template <class T, class I>
I CUtlRBTree<T, I>::FirstPreorder() const
{
	return m_Root;
}

template <class T, class I>
I CUtlRBTree<T, I>::NextPreorder(I i) const
{
	if (LeftChild(i) != InvalidIndex())
		return LeftChild(i);

	if (RightChild(i) != InvalidIndex())
		return RightChild(i);

	I parent = Parent(i);
	while (parent != InvalidIndex())
	{
		if (IsLeftChild(i) && (RightChild(parent) != InvalidIndex()))
			return RightChild(parent);
		i = parent;
		parent = Parent(parent);
	}
	return InvalidIndex();
}

template <class T, class I>
I CUtlRBTree<T, I>::PrevPreorder(I i) const
{
	assert(0);
	return InvalidIndex();
}

template <class T, class I>
I CUtlRBTree<T, I>::LastPreorder() const
{
	I i = m_Root;
	while (1)
	{
		while (RightChild(i) != InvalidIndex())
			i = RightChild(i);

		if (LeftChild(i) != InvalidIndex())
			i = LeftChild(i);
		else
			break;
	}
	return i;
}

template <class T, class I>
I CUtlRBTree<T, I>::FirstPostorder() const
{
	I i = m_Root;
	while (!IsLeaf(i))
	{
		if (LeftChild(i))
			i = LeftChild(i);
		else
			i = RightChild(i);
	}
	return i;
}

template <class T, class I>
I CUtlRBTree<T, I>::NextPostorder(I i) const
{
	I parent = Parent(i);
	if (parent == InvalidIndex())
		return InvalidIndex();

	if (IsRightChild(i))
		return parent;

	if (RightChild(parent) == InvalidIndex())
		return parent;

	i = RightChild(parent);
	while (!IsLeaf(i))
	{
		if (LeftChild(i))
			i = LeftChild(i);
		else
			i = RightChild(i);
	}
	return i;
}


template <class T, class I>
void CUtlRBTree<T, I>::Reinsert(I elem)
{
	Unlink(elem);
	Link(elem);
}

template <class T, class I>
int CUtlRBTree<T, I>::Depth(I node) const
{
	if (node == InvalidIndex())
		return 0;

	int depthright = Depth(RightChild(node));
	int depthleft = Depth(LeftChild(node));
	return max(depthright, depthleft) + 1;
}

template <class T, class I>
bool CUtlRBTree<T, I>::IsValid() const
{
	if (!Count())
		return true;

	if ((Root() >= MaxElement()) || (Parent(Root()) != InvalidIndex()))
		goto InvalidTree;

#ifdef UTLTREE_PARANOID

	int numFree = 0;
	int curr = m_FirstFree;
	while (curr != InvalidIndex())
	{
		++numFree;
		curr = RightChild(curr);
		if ((curr > MaxElement()) && (curr != InvalidIndex()))
			goto InvalidTree;
	}
	if (MaxElement() - numFree != Count())
		goto InvalidTree;

	int numFree2 = 0;
	for (curr = 0; curr < MaxElement(); ++curr)
	{
		if (!IsValidIndex(curr))
			++numFree2;
		else
		{
			int right = RightChild(curr);
			int left = LeftChild(curr);
			if ((right == left) && (right != InvalidIndex()))
				goto InvalidTree;

			if (right != InvalidIndex())
			{
				if (!IsValidIndex(right))
					goto InvalidTree;
				if (Parent(right) != curr)
					goto InvalidTree;
				if (IsRed(curr) && IsRed(right))
					goto InvalidTree;
			}

			if (left != InvalidIndex())
			{
				if (!IsValidIndex(left))
					goto InvalidTree;
				if (Parent(left) != curr)
					goto InvalidTree;
				if (IsRed(curr) && IsRed(left))
					goto InvalidTree;
			}
		}
	}
	if (numFree2 != numFree)
		goto InvalidTree;

#endif

	return true;

InvalidTree:
	return false;
}

template <class T, class I>
void CUtlRBTree<T, I>::SetLessFunc(typename CUtlRBTree<T, I>::LessFunc_t func)
{
	if (!m_LessFunc)
		m_LessFunc = func;
	else
	{
		assert(0);
	}
}

template <class T, class I>
void CUtlRBTree<T, I>::FindInsertionPosition(T const &insert, I &parent, bool &leftchild)
{
	I current = m_Root;
	parent = InvalidIndex();
	leftchild = false;
	while (current != InvalidIndex())
	{
		parent = current;
		if (m_LessFunc(insert, Element(current)))
		{
			leftchild = true; current = LeftChild(current);
		}
		else
		{
			leftchild = false; current = RightChild(current);
		}
	}
}

template <class T, class I>
I CUtlRBTree<T, I>::Insert(T const &insert)
{
	I parent;
	bool leftchild;
	FindInsertionPosition(insert, parent, leftchild);
	I newNode = InsertAt(parent, leftchild);
	CopyConstruct(&Element(newNode), insert);
	return newNode;
}


template <class T, class I>
void CUtlRBTree<T, I>::Insert(const T *pArray, int nItems)
{
	while (nItems--)
	{
		Insert(*pArray++);
	}
}

template <class T, class I>
I CUtlRBTree<T, I>::Find(T const &search) const
{
	I current = m_Root;
	while (current != InvalidIndex())
	{
		if (m_LessFunc(search, Element(current)))
			current = LeftChild(current);
		else if (m_LessFunc(Element(current), search))
			current = RightChild(current);
		else
			break;
	}
	return current;
}

#endif