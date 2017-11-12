/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
  friend class DList<T>;
  friend class DList<T>::iterator;

  DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
    _data(d), _prev(p), _next(n) {}

  // [NOTE] DO NOT ADD or REMOVE any data member
  T              _data;
  DListNode<T>*  _prev;
  DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
  DList() :_isSorted(true){
    _head = new DListNode<T>(T());
    _head->_prev = _head->_next = _head; // _head is a dummy node
  }
  ~DList() { clear(); delete _head; }

  // DO NOT add any more data member or function for class iterator
  class iterator
  {
    friend class DList;

  public:
    iterator(DListNode<T>* n= 0): _node(n) {}
    iterator(const iterator& i) : _node(i._node) {}
    ~iterator() {} // Should NOT delete _node

    // implement these overloaded operators
    const T& operator * () const { return _node->_data; }
    T& operator * () { return _node->_data; }
    iterator& operator ++ () { _node = _node->_next; return *this; }
    iterator operator ++ (int) { iterator tmp(*this); ++(*this); return tmp; } //post
    iterator& operator -- () { _node = _node->_prev; return *this; }
    iterator operator -- (int) { iterator tmp(*this); --(*this); return tmp; } //post

    iterator& operator = (const iterator& i) { _node = i._node; return *this; }

    bool operator != (const iterator& i) const { return _node != i._node; }
    bool operator == (const iterator& i) const { return _node == i._node; }

  private:
    DListNode<T>* _node;
  };

  // implement these functions
  iterator begin() const { return _head->_next; }
  iterator end() const { return _head; }
  bool empty() const { return _head->_next == _head; }
  size_t size() const {
    size_t sum = 0;
    for(iterator it=begin(); it!=end(); ++it)
      ++ sum;
    return sum;
  }

  void push_back(const T& x) { insert(end(), x); _isSorted = false; }
  void pop_front() { erase(begin()); }
  void pop_back()  { iterator e = --end(); erase(e); }

  // return false if nothing to erase
  bool erase(iterator pos) { _isSorted = false; return remove(pos._node); }
  bool erase(const T& x) {
    for(iterator it=begin(); it!=end(); ++it)
      if (*it == x)
        return erase(it);
    return false;
  }

  void clear() { while(!empty()) erase(begin()); _isSorted = true; }  // delete all nodes except for the dummy node

  void sort() const { if (!_isSorted){ stable_sort(begin(), end()); _isSorted = true;} }

private:
  // [NOTE] DO NOT ADD or REMOVE any data member
  DListNode<T>*  _head;     // = dummy node if list is empty
  mutable bool   _isSorted; // (optionally) to indicate the array is sorted

  // helper functions; called by public member functions
  DListNode<T>* insert(DListNode<T>* node, const T& x) { // insert before node
    DListNode<T>* newNode = new DListNode<T>(x, node->_prev, node);
    return insert(node, newNode);
  }
  DListNode<T>* insert(iterator it, const T& x) {
    return insert(it._node, x);
  }
  DListNode<T>* insert(iterator it, DListNode<T>* newNode) const {
    return insert(it._node, newNode);
  }
  DListNode<T>* insert(DListNode<T>* node, DListNode<T>* newNode) const {
    newNode->_prev = node->_prev; // double check
    newNode->_next = node;        // double check
    newNode->_next->_prev = newNode;
    newNode->_prev->_next = newNode;
    return newNode;
  }
  DListNode<T>* moveOut(iterator it) const {
    return moveOut(it._node);
  }
  DListNode<T>* moveOut(DListNode<T>* node) const { assert(node != _head);
    node->_next->_prev = node->_prev;
    node->_prev->_next = node->_next;
    return node;
  }
  bool remove(DListNode<T>* node) {
    if (node == _head)
      return false;
    delete moveOut(node);
    return true;
  }
  iterator stable_sort(iterator it_start, iterator it_end) const {
    // find middle for separting array
    size_t sum = 0;
    iterator it_mid = it_start;
    for(iterator it=it_start; it!=it_end; ++it) {
      if (++sum % 2 == 0)
        ++it_mid;
    }

    // end condition
    assert(sum > 0);
    if (sum <= 1)
      return it_start;

    // divide and concour
    it_start = stable_sort(it_start, it_mid);
    it_mid = stable_sort(it_mid, it_end);
    iterator it_return = it_start;
    --it_return;

    // merge it
    size_t num_start = sum / 2,
           num_end = sum - num_start;
    while (num_start > 0 && num_end > 0) {
      if (*it_start <= *it_mid) {
        --num_start;
        ++it_start;
      }
      else {
        --num_end;
        iterator tmp = it_mid++;
        insert(it_start, moveOut(tmp));
      }
    }
    return ++it_return;
  }

};

#endif // DLIST_H
