/*
 *
 * Author: Kevin XU <kevin.xu.1982.02.06@gmail.com>
 *
 */

#include <cassert>
#include <cstdlib>

#include <iostream>

#include "../include/ref-counting.hpp"

#include "rand.hpp"


#define _RAND_MAX 1024



using namespace std;

using namespace Test;

static bool _destructed = false;

class _TestRefCounting: public RefCounting {
public:
  _TestRefCounting(void) = default;

protected:
  ~_TestRefCounting() noexcept
  {
    _destructed = true;
  }
};

static_assert(RefCounting::CHECK<_TestRefCounting>::value, "");

int main(int argc, char const *argv[])
{
  _TestRefCounting *trc = new _TestRefCounting();

  int n = rand(_RAND_MAX);

  for (int i = 0; i < n; ++i)
    trc->ref();

  for (int i = 0; i < n; ++i)
    trc->deref();

  assert(!_destructed);

  n = rand(_RAND_MAX);

  for (int i = 0; i < n; ++i) {
    _TestRefCounting *_trc = trc->ref<_TestRefCounting>();

    assert(_trc == trc);
  }

  for (int i = 0; i < n; ++i)
    trc->deref();

  assert(!_destructed);

  n = rand(_RAND_MAX);

  for (int i = 0; i < n; ++i) {
    _TestRefCounting const *_trc = trc->ref<_TestRefCounting const>();

    assert(_trc == trc);
  }

  for (int i = 0; i < n; ++i)
    trc->deref();

  assert(!_destructed);

  _TestRefCounting const *ctrc = const_cast<_TestRefCounting const *>(trc);

  n = rand(_RAND_MAX);

  for (int i = 0; i < n; ++i) {
    _TestRefCounting const *_ctrc = ctrc->ref<_TestRefCounting>();

    assert(_ctrc == ctrc);
  }

  for (int i = 0; i < n; ++i)
    ctrc->deref();

  assert(!_destructed);

  n = rand(_RAND_MAX);

  for (int i = 0; i < n; ++i) {
    _TestRefCounting const *_ctrc = ctrc->ref<_TestRefCounting const>();

    assert(_ctrc == ctrc);
  }

  for (int i = 0; i < n; ++i)
    ctrc->deref();

  assert(!_destructed);

  trc->deref();

  assert(_destructed);

  cout << "\"test-ref-counting\" passed." << endl;

  return 0;
}

