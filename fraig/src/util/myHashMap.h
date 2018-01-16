/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
//
//    size_t operator() () const { return 0; }
//
//    bool operator == (const HashKey& k) const { return true; }
//
// private:
// };
//
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;
typedef vector<HashNode> VHashNode;

public:
  HashMap(size_t b=0) : _numBuckets(0), _buckets(NULL) { if (b != 0) init(b); }
  ~HashMap() { reset(); }

  void init(size_t b) {
    reset();
    _numBuckets = b;
    _buckets = new VHashNode[b];
  }

  void reset() {
    _numBuckets = 0;
    if (_buckets) {
      delete [] _buckets;
      _buckets = NULL;
    }
  }

  // little modified from hashSet
  HashData* insert(const HashKey& k, const HashData& d) {
    VHashNode &v = _buckets[bucketNum(k)];
    typename VHashNode::iterator it = find_if(v.begin(), v.end(),
      [&](HashNode& p) { return *(p.first) == *k; });
    if (it != v.end())
      return &it->second;
    v.push_back(HashNode(k, d));
    return NULL;
  }

private:
  size_t                   _numBuckets;
  VHashNode*               _buckets;

  size_t bucketNum(const HashKey& k) const { return ((*k)() % _numBuckets); }
};


// TODO: (Optionally) Implement your own Cache classes.

//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//
//    size_t operator() () const { return 0; }
//
//    bool operator == (const CacheKey&) const { return true; }
//
// private:
// };
//
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
  Cache() : _size(0), _cache(0) {}
  Cache(size_t s) : _size(0), _cache(0) { init(s); }
  ~Cache() { reset(); }

  // NO NEED to implement Cache::iterator class

  // TODO: implement these functions
  //
  // Initialize _cache with size s
  void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
  void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

  size_t size() const { return _size; }

  CacheNode& operator [] (size_t i) { return _cache[i]; }
  const CacheNode& operator [](size_t i) const { return _cache[i]; }

  // return false if cache miss
  bool read(const CacheKey& k, CacheData& d) const {
    size_t i = k() % _size;
    if (k == _cache[i].first) {
      d = _cache[i].second;
      return true;
    }
    return false;
  }
  // If k is already in the Cache, overwrite the CacheData
  void write(const CacheKey& k, const CacheData& d) {
    size_t i = k() % _size;
    _cache[i].first = k;
    _cache[i].second = d;
  }

private:
  // Do not add any extra data member
  size_t         _size;
  CacheNode*     _cache;
};


#endif // MY_HASH_H
