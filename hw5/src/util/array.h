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
  Array() : _data(new T[1]), _size(0), _capacity(1), _isSorted(true) {}
  ~Array() { delete []_data; }

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
    if ( _size + 1 == _capacity)
      reserve(_capacity << 1);
    _isSorted = false;
    _data[_size++] = x;
  }
  void pop_front() { 
    _isSorted = false;
    assert(_size > 0);
    _data[0] = _data[--_size];
  }
  void pop_back() { 
    assert(_size > 0);
    --_size;
  }
  bool erase(iterator pos) {
    _isSorted = false;
    if (!(pos._node < _data + _size && _data <= pos._node))
      return false;
    _data[pos._node - _data] = _data[--_size];
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
    reserve(1);
  }
  void sort() const { 
    if (!empty() && !_isSorted) { 
      ::sort(_data, _data+_size);
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

  // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
