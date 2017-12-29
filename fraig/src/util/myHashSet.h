/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>
#include <algorithm>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//

// template <class Data> class HashSet; // no used

template <class Data>
class HashSet
{
public:
  HashSet(size_t b = 0) : _numBuckets(0), _size(0), _buckets(0) { if (b != 0) init(b); }
  ~HashSet() { reset(); }

  // implement the HashSet<Data>::iterator
  // o An iterator should be able to go through all the valid Data
  //   in the Hash
  // o Functions to be implemented:
  //   - constructor(s), destructor
  //   - operator '*': return the HashNode
  //   - ++/--iterator, iterator++/--
  //   - operators '=', '==', !="
  //
  class iterator
  {
    friend class HashSet<Data>;

  public:
    iterator(const HashSet<Data>* hash=NULL, size_t num=0, size_t vnum=0)
      :_hash(hash), _num(num), _vnum(vnum) {};
    // const Data& operator * () const { return _hash[_num][_vnum]; }  // why
    const Data& operator * () const { return _hash->_buckets[_num][_vnum]; }
    iterator& operator ++ () { return go(); }
    bool operator != (const iterator& i) const { return _num != i._num || _vnum !=i._vnum; }
  private:
    const HashSet<Data>* _hash;
    size_t _num, _vnum;

    iterator& go() {
      assert(_hash != NULL);
      assert(_num <= _hash->numBuckets());
      ++_vnum;
      if (_vnum >= _hash->_buckets[_num].size()) {
        ++_num;
        while ((!_hash->_buckets[_num].size()) && _num < _hash->numBuckets())
          ++_num;
        _vnum = 0;
      }
      return *this;
    }
  };
  // friend class HashSet<Data>::iterator; not use

  void init(size_t b) {
    _numBuckets = b;
    _size = 0;
    _buckets = new vector<Data>[b];
  }
  void reset() {
    _numBuckets = 0;
    if (_buckets) {
      delete [] _buckets;
      _buckets = 0;
    }
  }
  void clear() {
    for (size_t i = 0; i < _numBuckets; ++i)
      _buckets[i].clear();
  }
  size_t numBuckets() const { return _numBuckets; }

  vector<Data>& operator [] (size_t i) { return _buckets[i]; }
  const vector<Data>& operator [] (size_t i) const { return _buckets[i]; }
  // Point to the first valid data
  iterator begin() const { return ++iterator(this, 0, -1); }
  // Pass the end
  iterator end() const { return iterator(this, numBuckets(), 0); }

  // return true if no valid data
  bool empty() const { return _size == 0; }
  // number of valid data
  size_t size() const { return _size; }

#define findIt() \
  vector<Data> &v = _buckets[bucketNum(d)]; \
  typename vector<Data>::iterator it = find(v.begin(), v.end(), d);

  // check if d is in the hash...
  // if yes, return true;
  // else return false;
  bool check(const Data& d) const {
    findIt();
    return it != v.end();
  }

  // query if d is in the hash...
  // if yes, replace d with the data in the hash and return true;
  // else return false;
  bool query(Data& d) const {
    findIt();
    if (it != v.end())
      d = *it;
    return it != v.end();
  }

  // update the entry in hash that is equal to d (i.e. == return true)
  // if found, update that entry with d and return true;
  // else insert d into hash as a new entry and return false;
  bool update(const Data& d) {
    findIt();
    if (it != v.end())
      *it = d;
    return it != v.end();
  }

  // return true if inserted successfully (i.e. d is not in the hash)
  // return false is d is already in the hash ==> will not insert
  bool insert(const Data& d) {
    findIt();
    if (it != v.end())
      return false;
    v.push_back(d);
    ++_size;
    return true;
  }

  // return true if removed successfully (i.e. d is in the hash)
  // return fasle otherwise (i.e. nothing is removed)
  bool remove(const Data& d) {
    findIt();
    if (it == v.end())
      return false;
    v.erase(it);
    --_size;
    return true;
  }

private:
  // Do not add any extra data member
  size_t            _numBuckets;
  size_t            _size;
  vector<Data>*     _buckets;

  size_t bucketNum(const Data& d) const { return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
