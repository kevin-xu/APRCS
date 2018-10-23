/*
 *
 * Author: Kevin XU <kevin.xu.1982.02.06@gmail.com>
 *
 */

#include <cassert>
#include <cstdlib>

#include <iostream>

#include <ref-counting.hpp>

#include "rand.hpp"


#define _RAND_MAX 1024



using namespace std;

using namespace Test;

static bool _destructed = false;

class _TestRefCountingObject: public RefCounting {
public:
  _TestRefCountingObject(void) = default;

protected:
  ~_TestRefCountingObject() noexcept
  {
    _destructed = true;
  }
};

static_assert(RefCounting::CHECK<_TestRefCountingObject>::value, "");

int main(int argc, char const *argv[])
{
  _TestRefCountingObject *trco = new _TestRefCountingObject();

  int n = rand(_RAND_MAX);

  for (int i = 0; i < n; ++i)
    trco->ref();

  for (int i = 0; i < n; ++i)
    trco->deref();

  assert(!_destructed);

  n = rand(_RAND_MAX);

  for (int i = 0; i < n; ++i) {
    _TestRefCountingObject *_trco = trco->ref<_TestRefCountingObject>();

    assert(_trco == trco);
  }

  for (int i = 0; i < n; ++i)
    trco->deref();

  assert(!_destructed);

  n = rand(_RAND_MAX);

  for (int i = 0; i < n; ++i) {
    _TestRefCountingObject const *_trco = trco->ref<_TestRefCountingObject const>();

    assert(_trco == trco);
  }

  for (int i = 0; i < n; ++i)
    trco->deref();

  assert(!_destructed);

  _TestRefCountingObject const *ctrco = const_cast<_TestRefCountingObject const *>(trco);

  n = rand(_RAND_MAX);

  for (int i = 0; i < n; ++i) {
    _TestRefCountingObject const *_ctrco = ctrco->ref<_TestRefCountingObject>();

    assert(_ctrco == ctrco);
  }

  for (int i = 0; i < n; ++i)
    ctrco->deref();

  assert(!_destructed);

  n = rand(_RAND_MAX);

  for (int i = 0; i < n; ++i) {
    _TestRefCountingObject const *_ctrco = ctrco->ref<_TestRefCountingObject const>();

    assert(_ctrco == ctrco);
  }

  for (int i = 0; i < n; ++i)
    ctrco->deref();

  assert(!_destructed);

  trco->deref();

  assert(_destructed);

  cout << "\"test-ref-counting\" passed." << endl;

  return 0;
}

