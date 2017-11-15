/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
  friend class BSTree<T>;
  friend class BSTree<T>::iterator;

  BSTreeNode(const T& d, BSTreeNode<T>* p = NULL, BSTreeNode<T>* l = NULL, BSTreeNode<T>* r = NULL)
    :_data(d), _p(p) {
      _lr[0] = l;
      _lr[1] = r;
  }
  const T& operator * () const { return _data; }
  T& operator * () { return _data; }

  T               _data;
  BSTreeNode<T>*  _p;
  BSTreeNode<T>*  _lr[2];
};


template <class T>
class BSTree
{
public:
  BSTree():_size(0) {
    _head = new BSTreeNode<T>(T(string(1, char(127)))); // dummy largest
    _head->_p = _head->_lr[1] = _head;
  }
  ~BSTree() { clear(); delete _head; }
  class iterator
  {
    friend class BSTree;

  public:
    iterator(BSTreeNode<T>* n=NULL): _node(n) {}
    iterator(const iterator& i): _node(i._node) {}
    ~iterator() {}

    const T& operator * () const { return **_node; }
    T& operator * () { return **_node; }
    iterator& operator ++ () { go(true); return *this; }
    iterator operator ++ (int) { iterator tmp(*this); ++(*this); return tmp; } //post
    iterator& operator -- () { go(false); return *this; }
    iterator operator -- (int) { iterator tmp(*this); --(*this); return tmp; } //post

    iterator& operator = (const iterator& i) { _node = i._node; return *this; }

    bool operator != (const iterator& i) const { return _node != i._node; }
    bool operator == (const iterator& i) const { return _node == i._node; }

    BSTreeNode<T>* getNode() const { return _node; }

  private:
    BSTreeNode<T>* _node;

    void go(bool forward) {
      // go to the first larger element in tree
      if (_node->_lr[forward]) {
        _node = _node->_lr[forward];
        while (_node->_lr[!forward])
          _node = _node->_lr[!forward];
      }
      else { // go up
        BSTreeNode<T>* prev = _node;
        do {
          prev = _node;
          _node = _node->_p;
        } while (_node->_lr[forward] == prev);
      }
    }
  };

  iterator begin() const { return ++iterator(_head); }
  iterator end() const { return _head; }
  bool empty() const { return !_head->_lr[0]; }
  size_t size() const { return _size; }

  void insert(const T& x) { insert(find(x, true), x); }

  // Base on erase iterator
  void pop_front() { erase(begin()); }
  void pop_back()  { erase(--end()); }
  bool erase(iterator it_pos) {
    // empty
    BSTreeNode<T>* node_pos = it_pos.getNode();
    if (node_pos  == _head)
      return false;

    // is leaf -> remove
    if (leafRemove(node_pos))
      return remove(node_pos);
    // is node -> get replace element
    iterator it_next = it_pos;
    ++it_next;
    BSTreeNode<T>* node_next = it_next.getNode();

    // move out -> substitude
    if (leafRemove(node_next)) {
      node_next->_p = node_pos->_p;
      setReverse(node_pos, node_next);
      node_next->_lr[0] = node_pos->_lr[0];
      node_next->_lr[1] = node_pos->_lr[1];
      if (node_next->_lr[0])
        node_next->_lr[0]->_p = node_next;
      if (node_next->_lr[1])
        node_next->_lr[1]->_p = node_next;
      return remove(node_pos);
    }
    else
      assert(false);
    return false;
  }
  bool erase(const T& x) {
    BSTreeNode<T>* f = find(x);
    if (**f == x)
      return erase(f);
    return false;
  }
  void clear() {
    recurClear(_head->_lr[0]);
    _head->_lr[0] = NULL;
    _size = 0;
  }
  void print() const { recurPrint(_head->_lr[0]); }
  void sort() const { return; }
private:
  BSTreeNode<T>*  _head;     // dummy (the largest)
  size_t          _size;

  BSTreeNode<T>* find(const T& x, bool strict=false) {
    BSTreeNode<T>* now = _head;
    while (x != **now || (strict && now->_lr[0])) {
      if (!now->_lr[x > **now]) // return parent
        return now;
      now = now->_lr[x > **now];
    }
    return now; // return find
  }
  void insert(BSTreeNode<T>* parent, const T& x) {
    assert(parent->_lr[x > **parent] == NULL);
    parent->_lr[x > **parent] = new BSTreeNode<T>(x, parent);
    ++_size;
  }

  // get reversed link from child to parent
  void setReverse(BSTreeNode<T>* x, BSTreeNode<T> *want) {
    for (int i=0; i<2; ++i)
      if (x->_p->_lr[i] == x) {
        x->_p->_lr[i] = want;
        return ;
      }
    assert(false);
  }
  bool leafRemove(BSTreeNode<T>* node_pos) {
    int num = 0;
    if (node_pos->_lr[0]) num |= 1;
    if (node_pos->_lr[1]) num |= 2;
    // no leaf
    if (num == 0) {
      setReverse(node_pos, NULL);
      return true;
    }
    // one leaf
    else if (num != 3) {
      setReverse(node_pos, node_pos->_lr[num == 2]);
      node_pos->_lr[num == 2]->_p = node_pos->_p;
      return true;
    }
    else
      return false;
  }
  bool remove(BSTreeNode<T>* t) {
    assert(t);
    delete t;
    --_size;
    return true;
  }
  void recurClear(BSTreeNode<T>* t) {
    if (!t) return;
    recurClear(t->_lr[0]);
    recurClear(t->_lr[1]);
    delete t;
  }
  void recurPrint(BSTreeNode<T>* t, int n=0) const {
    if (!t) {
      cout << string(n * 2, ' ') << "[0]\n";
      return;
    }
    cout << string(n * 2, ' ') << **t << endl;
    recurPrint(t->_lr[0], n + 1);
    recurPrint(t->_lr[1], n + 1);
  }

};

#endif // BST_H
