/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

#define getParent(i) (((i + 1) >> 1) - 1)
#define getChild(i)  (((i + 1) << 1) - 1)

template <class Data>
class MinHeap
{
public:
  MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
  ~MinHeap() {}

  void clear() { _data.clear(); }

  // For the following member functions,
  // We don't respond for the case vector "_data" is empty!
  const Data& operator [] (size_t i) const { return _data[i]; }   
  Data& operator [] (size_t i) { return _data[i]; }

  size_t size() const { return _data.size(); }

  const Data& min() const { return _data.front(); }
  void insert(const Data& d) {
    size_t i = size();
    _data.push_back(d);
    // flow up
    while (i && _data[i] < _data[getParent(i)]) {
      swap(_data[i], _data[getParent(i)]);
      i = getParent(i);
    }
  }
  void delMin() { delData(0); }
  void delData(size_t i) {
    assert(size() > 0);
    swap(_data[i], _data[size() - 1]);
    _data.pop_back();

    // flow up
    while (i && _data[i] < _data[getParent(i)]) {
      swap(_data[i], _data[getParent(i)]);
      i = getParent(i);
    }
    // sink down
    while (getChild(i) < size()) {
      size_t small = getChild(i);
      if (small + 1 < size() && _data[small + 1] < _data[small])
        ++small;
      if (_data[i] < _data[small])
        break;
      swap(_data[i], _data[small]);
      i = small;
    }
  }

private:
  // DO NOT add or change data members
  vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
