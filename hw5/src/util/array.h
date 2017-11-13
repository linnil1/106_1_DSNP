/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
  Array() : _data(NULL), _size(0), _capacity(0), _isSorted(true) {}
  ~Array() { if(_capacity) delete []_data; }

  // DO NOT add any more data member or function for class iterator
  class iterator
  {
    friend class Array;

  public:
    iterator(T* n= 0): _node(n) {}
    iterator(const iterator& i): _node(i._node) {}
    ~iterator() {} // Should NOT delete _node

    // implement these overloaded operators
    const T& operator * () const { return *_node; }
    T& operator * () { return *_node; }
    iterator& operator ++ () { ++_node; return *this; } // pre
    iterator operator ++ (int) { iterator tmp(*this); ++_node; return tmp; } //post
    iterator& operator -- () { --_node; return *this; } // pre
    iterator operator -- (int) { iterator tmp(*this); --_node; return tmp; } //post

    iterator operator + (int i) const { iterator tmp(*this); tmp._node += i; return tmp; }
    iterator& operator += (int i) { _node += i; return (*this); }

    iterator& operator = (const iterator& i) { _node = i._node; return *this; }

    bool operator != (const iterator& i) const { return _node != i._node; }
    bool operator == (const iterator& i) const { return _node == i._node; }

  private:
    T*    _node;
  };

  iterator begin() const { return _data; }
  iterator end() const { return _data + _size; }
  bool empty() const { return _size == 0; }
  size_t size() const { return _size; }
  T& operator [] (size_t i) { return _data[i]; }
  const T& operator [] (size_t i) const { return _data[i]; }

  void push_back(const T& x) {
    if (_size + 1 > _capacity) {
      if (_capacity == 0)
        reserve(_size + 1);
      else
        reserve(_capacity << 1);
    }
    _isSorted = false;
    _data[_size++] = x;
  }
  void pop_front() {
    if (_size <= 0)
        return ;
    erase(begin());
  }
  void pop_back() {
    if (_size <= 0)
        return ;
    --_size;
  }
  bool erase(iterator pos) {
    _isSorted = false;
    if (!(getNode(pos) < _data + _size && _data <= getNode(pos)))
      return false;
    _data[getIndex(pos, begin())] = _data[--_size];
    return true;
  }
  bool erase(const T& x) {
    _isSorted = false;
    for(iterator it = begin(); it != end(); ++it)
      if (*it == x)
        return erase(it);
    return false;
  }
  void clear() {
    _size = 0;
    _isSorted = true;
    reserve(0);
  }
  void sort() const {
    if (!empty() && !_isSorted) {
      //stable_sort(_data, end());
      mySort(begin(), end());
      _isSorted = true;
    }
  }

  // Nice to have, but not required in this homework...
  void reserve(size_t n) {
    if (n < 1 && n <= _size)
      return ;
    T* tmpdata = new T[n];
    int i = 0;
    for(iterator it = begin(); it != end(); ++it, ++i)
      tmpdata[i] = *it;
    delete[] _data;
    _data = tmpdata;
    _capacity = n;
  }
  // void resize(size_t n) { ... }

private:
  // [NOTE] DO NOT ADD or REMOVE any data member
  T*            _data;
  size_t        _size;       // number of valid elements
  size_t        _capacity;   // max number of elements
  mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

  // Helper functions; called by public member functions
  // it should be written in iterator
  size_t getIndex(const iterator &a, const iterator &b) const { return getNode(a) - getNode(b); }
  T* getNode(const iterator &a) const { return a._node; }

  void mySort(iterator it_start, iterator it_end) const {
    size_t sum = getIndex(it_end, it_start);
    if (sum <= 1)
      return ;
    // divide and concour
    iterator it_mid = it_start + sum / 2;
    mySort(it_start, it_mid);
    mySort(it_mid, it_end);

    // merge it with buffer
    // pre-process
    // this should use Array we defined, but use it will conflict urGen()
    size_t num_start = sum / 2, i = 0;
    T** temparr = new T* [num_start];
    for(iterator it = it_start; it != it_mid; ++it)
      temparr[i++] = new T(*it);

    // main merge
    for (size_t i=0; i<num_start; ++i) {
      while (it_mid != it_end && *temparr[i] > *it_mid)
        *(it_start++) = *(it_mid++);
      *(it_start++) = *temparr[i];
    }
    // delete it
    for (size_t i=0; i<num_start; ++i)
      delete temparr[i];
    delete[] temparr;
  }

};

#endif // ARRAY_H
