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
// the class "Pdata*" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//

// Pdata means pointer of data
template <class Pdata>
class HashSet
{
  typedef vector<Pdata> VDATA;
public:
  HashSet(size_t s=0):_buckets(NULL) { init(s); }
  ~HashSet() { reset(); }

  void init(size_t s) {
    _numBuckets = s;
    assert(!_buckets);
    if (s)
      _buckets = new VDATA[s];
  }
  void reset() {
    _numBuckets = 0;
    if (_buckets) {
      delete [] _buckets;
      _buckets = NULL;
    }
  }

  // return NULL if inserted successfully (i.e. d is not in the hash)
  // return pointer is d is already in the hash ==> will not insert and return
  Pdata insert(Pdata d) {
    VDATA &v = _buckets[bucketNum(d)];
    typename VDATA::iterator it = find_if(v.begin(), v.end(),
      [&](Pdata& p) { return *p == *d; });
    if (it != v.end())
      return *it;
    v.push_back(d);
    return NULL;
  }

private:
  // Do not add any extra data member
  size_t            _numBuckets;
  VDATA*            _buckets;

  size_t bucketNum(Pdata d) const { return ((*d)() % _numBuckets); }
};

#endif // MY_HASH_SET_H
