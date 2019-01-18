/*
 *
 * Author: Kevin XU <kevin.xu.1982.02.06@gmail.com>
 *
 */

#include <cassert>

#include <iostream>
#include <string>
#include <utility>

#include <auto-ptr.hpp>
#include <ref-counting.hpp>

#include "arguments.hpp"


#define _NEW__ARGUMENTS \
  '=', 7, 0.7f, 0.07, string("abc"), (void *)nullptr



using namespace std;

using namespace Test;

static Arguments _constructorArguments;

static unsigned _nConstructings = 0U;

static void *_constructed = nullptr;

static void *_destructed = nullptr;

static bool _done = false;

static void _recoverState(void)
{
  _constructorArguments();

  _nConstructings = 0U;

  _constructed = nullptr;

  _destructed = nullptr;

  _done = false;
}

class _TestRefCountingObject: public RefCounting {
public:
  _TestRefCountingObject(void) noexcept
  {
    ++_nConstructings;

    _constructed = this;
  }

  template <class ... Ts>
  _TestRefCountingObject(Ts... arguments)
  {
    _constructorArguments(arguments...);

    ++_nConstructings;

    _constructed = this;
  }

  void doo(void) const noexcept
  {
    _done = true;
  }

protected:
  ~_TestRefCountingObject() noexcept
  {
    _destructed = this;
  }
};

class _TestRefCountingObjectDerived: public _TestRefCountingObject {
public:
  _TestRefCountingObjectDerived(void) = default;

protected:
  ~_TestRefCountingObjectDerived() = default;
};

int main(int argc, char const *argv[])
{
  {
    AutoPtr<_TestRefCountingObject> trco;

    assert(_nConstructings == 0U);

    assert(trco == nullptr);
  }

  {
    AutoPtr<_TestRefCountingObject> trco = nullptr;

    assert(_nConstructings == 0U);

    assert(trco == nullptr);
  }

  {
    {
      _TestRefCountingObject *trco = new _TestRefCountingObject();

      AutoPtr<_TestRefCountingObject> _trco = trco;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(_trco == trco);

      trco->deref();

      assert(!_destructed);
    }

    assert(_destructed);

    _recoverState();
  }

  {
    {
      _TestRefCountingObject *trco = new _TestRefCountingObject();

      AutoPtr<_TestRefCountingObject> _trco = trco;

      AutoPtr<_TestRefCountingObject> __trco = _trco;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(__trco == trco);

      trco->deref();

      assert(!_destructed);
    }

    assert(_destructed);

    _recoverState();
  }

  {
    {
      _TestRefCountingObjectDerived *trcod = new _TestRefCountingObjectDerived();

      AutoPtr<_TestRefCountingObjectDerived> _trcod = trcod;

      AutoPtr<_TestRefCountingObject> trco = _trcod;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco == trcod);

      trcod->deref();

      assert(!_destructed);
    }

    assert(_destructed);

    _recoverState();
  }

  {
    {
      _TestRefCountingObject *trco = new _TestRefCountingObject();

      AutoPtr<_TestRefCountingObject> _trco = trco;

      AutoPtr<_TestRefCountingObject> __trco = move(_trco);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(__trco == trco);

      trco->deref();

      assert(!_destructed);
    }

    assert(_destructed);

    _recoverState();
  }

  {
    {
      _TestRefCountingObjectDerived *trcod = new _TestRefCountingObjectDerived();

      AutoPtr<_TestRefCountingObjectDerived> _trcod = trcod;

      AutoPtr<_TestRefCountingObject> trco = move(_trcod);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco == trcod);

      trcod->deref();

      assert(!_destructed);
    }

    assert(_destructed);

    _recoverState();
  }

  {
    {
      _TestRefCountingObject *trco = new _TestRefCountingObject();

      AutoPtr<_TestRefCountingObject> _trco = trco;

      trco->deref();
    }

    assert(_destructed);

    _recoverState();
  }

  {
    AutoPtr<_TestRefCountingObject> trco;

    trco = nullptr;

    assert(_nConstructings == 0U);

    assert(trco == nullptr);

    _TestRefCountingObject *_trco = new _TestRefCountingObject();

    AutoPtr<_TestRefCountingObject> __trco = _trco;

    _trco->deref();

    __trco = nullptr;

    assert(_nConstructings == 1U);

    assert(_destructed);

    assert(__trco == nullptr);

    _recoverState();
  }

  {
    {
      _TestRefCountingObject *trco = new _TestRefCountingObject();

      AutoPtr<_TestRefCountingObject> _trco;

      _trco = trco;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(_trco == trco);

      trco->deref();

      assert(!_destructed);

      _trco = nullptr;

      assert(_destructed);

      _recoverState();

      trco = new _TestRefCountingObject();

      AutoPtr<_TestRefCountingObject> __trco = trco;

      __trco = trco;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(__trco == trco);

      trco->deref();

      assert(!_destructed);

      __trco = nullptr;

      assert(_destructed);

      _recoverState();

      trco = new _TestRefCountingObject();

      AutoPtr<_TestRefCountingObject> ___trco = trco;

      trco->deref();

      _TestRefCountingObject *trco2 = new _TestRefCountingObject();

      ___trco = trco2;

      assert(_nConstructings == 2U);

      assert(_destructed == trco);

      assert(___trco == trco2);

      trco2->deref();

      assert(_destructed != trco2);

      ___trco = nullptr;

      assert(_destructed == trco2);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingObject> trco1;

      AutoPtr<_TestRefCountingObject> trco2;

      trco1 = trco2;

      assert(_nConstructings == 0U);

      assert(trco1 == nullptr);

      assert(trco2 == nullptr);

      _TestRefCountingObject *_trco1 = new _TestRefCountingObject();

      trco1 = _trco1;

      _trco1->deref();

      trco1 = trco2;

      assert(_nConstructings == 1U);

      assert(_destructed);

      assert(trco1 == nullptr);

      assert(trco2 == nullptr);

      _recoverState();

      _TestRefCountingObject *_trco2 = new _TestRefCountingObject();

      trco2 = _trco2;

      trco1 = trco2;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == _trco2);

      assert(trco2 == _trco2);

      _trco2->deref();

      assert(!_destructed);

      trco1 = nullptr;

      assert(!_destructed);

      trco2 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCountingObject *trco = new _TestRefCountingObject();

      trco1 = trco;

      trco2 = trco;

      trco1 = trco2;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == trco);

      assert(trco2 == trco);

      trco->deref();

      assert(!_destructed);

      trco1 = nullptr;

      assert(!_destructed);

      trco2 = nullptr;

      assert(_destructed);

      _recoverState();

      _trco1 = new _TestRefCountingObject();

      trco1 = _trco1;

      _trco1->deref();

      _trco2 = new _TestRefCountingObject();

      trco2 = _trco2;

      trco1 = trco2;

      assert(_nConstructings == 2U);

      assert(_destructed == _trco1);

      assert(trco1 == _trco2);

      assert(trco2 == _trco2);

      _trco2->deref();

      assert(_destructed != _trco2);

      trco1 = nullptr;

      assert(_destructed != _trco2);

      trco2 = nullptr;

      assert(_destructed == _trco2);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingObject> trco;

      AutoPtr<_TestRefCountingObjectDerived> trcod;

      trco = trcod;

      assert(_nConstructings == 0U);

      assert(trco == nullptr);

      assert(trcod == nullptr);

      _TestRefCountingObject *_trco = new _TestRefCountingObject();

      trco = _trco;

      _trco->deref();

      trco = trcod;

      assert(_nConstructings == 1U);

      assert(_destructed);

      assert(trco == nullptr);

      assert(trcod == nullptr);

      _recoverState();

      _TestRefCountingObjectDerived *_trcod = new _TestRefCountingObjectDerived();

      trcod = _trcod;

      trco = trcod;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco == _trcod);

      assert(trcod == _trcod);

      _trcod->deref();

      assert(!_destructed);

      trco = nullptr;

      assert(!_destructed);

      trcod = nullptr;

      assert(_destructed);

      _recoverState();

      _trcod = new _TestRefCountingObjectDerived();

      trcod = _trcod;

      trco = _trcod;

      trco = trcod;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco == _trcod);

      assert(trcod == _trcod);

      _trcod->deref();

      assert(!_destructed);

      trco = nullptr;

      assert(!_destructed);

      trcod = nullptr;

      assert(_destructed);

      _recoverState();

      _trco = new _TestRefCountingObject();

      trco = _trco;

      _trco->deref();

      _trcod = new _TestRefCountingObjectDerived();

      trcod = _trcod;

      trco = trcod;

      assert(_nConstructings == 2U);

      assert(_destructed == _trco);

      assert(trco == _trcod);

      assert(trcod == _trcod);

      _trcod->deref();

      assert(_destructed != _trcod);

      trco = nullptr;

      assert(_destructed != _trcod);

      trcod = nullptr;

      assert(_destructed == _trcod);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingObject> trco1;

      AutoPtr<_TestRefCountingObject> trco2;

      trco1 = move(trco2);

      assert(_nConstructings == 0U);

      assert(trco1 == nullptr);

      assert(trco2 == nullptr);

      _TestRefCountingObject *_trco1 = new _TestRefCountingObject();

      trco1 = _trco1;

      _trco1->deref();

      trco1 = move(trco2);

      assert(_nConstructings == 1U);

      assert(_destructed);

      assert(trco1 == nullptr);

      assert(trco2 == nullptr);

      _recoverState();

      _TestRefCountingObject *_trco2 = new _TestRefCountingObject();

      trco2 = _trco2;

      trco1 = move(trco2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == _trco2);

      _trco2->deref();

      assert(!_destructed);

      assert(trco2 == nullptr);

      trco1 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCountingObject *trco = new _TestRefCountingObject();

      trco1 = trco;

      trco2 = trco;

      trco1 = move(trco2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == trco);

      assert(trco2 == trco);

      trco->deref();

      assert(!_destructed);

      trco1 = nullptr;

      assert(!_destructed);

      trco2 = nullptr;

      assert(_destructed);

      _recoverState();

      _trco1 = new _TestRefCountingObject();

      trco1 = _trco1;

      _trco1->deref();

      _trco2 = new _TestRefCountingObject();

      trco2 = _trco2;

      trco1 = move(trco2);

      assert(_nConstructings == 2U);

      assert(_destructed == _trco1);

      assert(trco1 == _trco2);

      _trco2->deref();

      assert(_destructed != _trco2);

      assert(trco2 == nullptr);

      trco1 = nullptr;

      assert(_destructed == _trco2);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingObject> trco;

      AutoPtr<_TestRefCountingObjectDerived> trcod;

      trco = move(trcod);

      assert(_nConstructings == 0U);

      assert(trco == nullptr);

      assert(trcod == nullptr);

      _TestRefCountingObject *_trco = new _TestRefCountingObject();

      trco = _trco;

      _trco->deref();

      trco = move(trcod);

      assert(_nConstructings == 1U);

      assert(_destructed);

      assert(trco == nullptr);

      assert(trcod == nullptr);

      _recoverState();

      _TestRefCountingObjectDerived *_trcod = new _TestRefCountingObjectDerived();

      trcod = _trcod;

      trco = move(trcod);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco == _trcod);

      _trcod->deref();

      assert(!_destructed);

      assert(trcod == nullptr);

      trco = nullptr;

      assert(_destructed);

      _recoverState();

      _trcod = new _TestRefCountingObjectDerived();

      trcod = _trcod;

      trco = _trcod;

      trco = move(trcod);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco == _trcod);

      assert(trcod == _trcod);

      _trcod->deref();

      assert(!_destructed);

      trco = nullptr;

      assert(!_destructed);

      trcod = nullptr;

      assert(_destructed);

      _recoverState();

      _trco = new _TestRefCountingObject();

      trco = _trco;

      _trco->deref();

      _trcod = new _TestRefCountingObjectDerived();

      trcod = _trcod;

      trco = move(trcod);

      assert(_nConstructings == 2U);

      assert(_destructed == _trco);

      assert(trco == _trcod);

      _trcod->deref();

      assert(_destructed != _trcod);

      assert(trcod == nullptr);

      trco = nullptr;

      assert(_destructed == _trcod);
    }

    _recoverState();
  }

  {
  }

  {
  }

  {
  }

  {
  }

  {
  }

  {
  }

  {
    {
      _TestRefCountingObject *trco = new _TestRefCountingObject();

      AutoPtr<_TestRefCountingObject> _trco = trco;

      trco->deref();

      _trco->doo();

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(_done == true);

      _trco = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
    {
      _TestRefCountingObject *trco = new _TestRefCountingObject();

      AutoPtr<_TestRefCountingObject> _trco = trco;

      _TestRefCountingObject &__trco = *_trco;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(&__trco == trco);

      trco->deref();

      assert(!_destructed);

      _trco = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
  }

  {
    {
      AutoPtr<_TestRefCountingObject> trco;

      assert((_TestRefCountingObject *)trco == nullptr);

      assert(_nConstructings == 0U);

      trco = nullptr;

      assert(trco == nullptr);

      assert(_nConstructings == 0U);

      _TestRefCountingObject *_trco = new _TestRefCountingObject();

      trco = _trco;

      assert((_TestRefCountingObject *)trco == _trco);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      _trco->deref();

      assert(!_destructed);

      trco = nullptr;

      assert(_destructed);

      _recoverState();

      _trco = new _TestRefCountingObject();

      trco = _trco;

      assert(trco == _trco);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      _trco->deref();

      assert(!_destructed);

      trco = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingObjectDerived> trcod;

      assert((_TestRefCountingObject *)trcod == nullptr);

      assert(_nConstructings == 0U);

      trcod = nullptr;

      assert(trcod == (_TestRefCountingObject *)nullptr);

      assert(_nConstructings == 0U);

      _TestRefCountingObjectDerived *_trcod = new _TestRefCountingObjectDerived();

      trcod = _trcod;

      assert((_TestRefCountingObject *)trcod == _trcod);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      _trcod->deref();

      assert(!_destructed);

      trcod = nullptr;

      assert(_destructed);

      _recoverState();

      _trcod = new _TestRefCountingObjectDerived();

      trcod = _trcod;

      assert(trcod == (_TestRefCountingObject *)_trcod);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      _trcod->deref();

      assert(!_destructed);

      trcod = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingObject> trco;

      assert((_TestRefCountingObjectDerived *)trco == nullptr);

      assert(_nConstructings == 0U);

      _TestRefCountingObjectDerived *trcod = new _TestRefCountingObjectDerived();

      trco = trcod;

      assert((_TestRefCountingObjectDerived *)trco == trcod);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      trcod->deref();

      assert(!_destructed);

      trco = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingObject> trco;

      assert(!(bool)trco);

      assert(_nConstructings == 0U);

      trco = nullptr;

      assert(!trco);

      assert(_nConstructings == 0U);

      _TestRefCountingObject *_trco = new _TestRefCountingObject();

      trco = _trco;

      _trco->deref();

      assert((bool)trco);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      trco = nullptr;

      assert(_destructed);

      _recoverState();

      _trco = new _TestRefCountingObject();

      trco = _trco;

      _trco->deref();

      assert(trco);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      trco = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingObject> trco1;

      AutoPtr<_TestRefCountingObject> trco2;

      trco1.swap(trco2);

      assert(_nConstructings == 0U);

      assert(trco1 == nullptr);

      assert(trco2 == nullptr);

      _TestRefCountingObject *_trco1 = new _TestRefCountingObject();

      trco1 = _trco1;

      trco1.swap(trco2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == nullptr);

      assert(trco2 == _trco1);

      _trco1->deref();

      assert(!_destructed);

      trco2 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCountingObject *_trco2 = new _TestRefCountingObject();

      trco2 = _trco2;

      trco1.swap(trco2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == _trco2);

      _trco2->deref();

      assert(!_destructed);

      assert(trco2 == nullptr);

      trco1 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCountingObject *trco = new _TestRefCountingObject();

      trco1 = trco;

      trco2 = trco;

      trco1.swap(trco2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == trco);

      assert(trco2 == trco);

      trco->deref();

      assert(!_destructed);

      trco1 = nullptr;

      assert(!_destructed);

      trco2 = nullptr;

      assert(_destructed);

      _recoverState();

      _trco1 = new _TestRefCountingObject();

      trco1 = _trco1;

      _trco2 = new _TestRefCountingObject();

      trco2 = _trco2;

      trco1.swap(trco2);

      assert(_nConstructings == 2U);

      assert(!_destructed);

      assert(trco1 == _trco2);

      _trco2->deref();

      assert(!_destructed);

      assert(trco2 == _trco1);

      _trco1->deref();

      assert(!_destructed);

      trco1 = nullptr;

      assert(_destructed == _trco2);

      trco2 = nullptr;

      assert(_destructed == _trco1);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingObject> trco1;

      AutoPtr<_TestRefCountingObject> trco2;

      assert(trco1 == trco2);

      assert(_nConstructings == 0U);

      _TestRefCountingObject *_trco1 = new _TestRefCountingObject();

      trco1 = _trco1;

      trco2 = nullptr;

      assert(!(trco1 == trco2));

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == _trco1);

      assert(trco2 == nullptr);

      _trco1->deref();

      assert(!_destructed);

      trco1 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCountingObject *_trco2 = new _TestRefCountingObject();

      trco1 = nullptr;

      trco2 = _trco2;

      assert(!(trco1 == trco2));

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == nullptr);

      assert(trco2 == _trco2);

      _trco2->deref();

      assert(!_destructed);

      trco2 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCountingObject *trco = new _TestRefCountingObject();

      trco1 = trco;

      trco2 = trco;

      trco->deref();

      assert(trco1 == trco2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      trco1 = nullptr;

      assert(!_destructed);

      trco2 = nullptr;

      assert(_destructed);

      _recoverState();

      _trco1 = new _TestRefCountingObject();

      trco1 = _trco1;

      _trco1->deref();

      _trco2 = new _TestRefCountingObject();

      trco2 = _trco2;

      _trco2->deref();

      assert(!(trco1 == trco2));

      assert(_nConstructings == 2U);

      assert(!_destructed);

      trco1 = nullptr;

      assert(_destructed == _trco1);

      trco2 = nullptr;

      assert(_destructed == _trco2);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingObject> trco1;

      AutoPtr<_TestRefCountingObject> trco2;

      assert(!(trco1 != trco2));

      assert(_nConstructings == 0U);

      _TestRefCountingObject *_trco1 = new _TestRefCountingObject();

      trco1 = _trco1;

      trco2 = nullptr;

      assert(trco1 != trco2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == _trco1);

      assert(trco2 == nullptr);

      _trco1->deref();

      assert(!_destructed);

      trco1 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCountingObject *_trco2 = new _TestRefCountingObject();

      trco1 = nullptr;

      trco2 = _trco2;

      assert(trco1 != trco2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trco1 == nullptr);

      assert(trco2 == _trco2);

      _trco2->deref();

      assert(!_destructed);

      trco2 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCountingObject *trco = new _TestRefCountingObject();

      trco1 = trco;

      trco2 = trco;

      trco->deref();

      assert(!(trco1 != trco2));

      assert(_nConstructings == 1U);

      assert(!_destructed);

      trco1 = nullptr;

      assert(!_destructed);

      trco2 = nullptr;

      assert(_destructed);

      _recoverState();

      _trco1 = new _TestRefCountingObject();

      trco1 = _trco1;

      _trco1->deref();

      _trco2 = new _TestRefCountingObject();

      trco2 = _trco2;

      _trco2->deref();

      assert(trco1 != trco2);

      assert(_nConstructings == 2U);

      assert(!_destructed);

      trco1 = nullptr;

      assert(_destructed == _trco1);

      trco2 = nullptr;

      assert(_destructed == _trco2);
    }

    _recoverState();
  }

  {
  }

  {
  }

  {
  }

  {
    NEW<_TestRefCountingObject>();

    assert(_nConstructings == 1U);

    assert(_constructed == _destructed);

    _recoverState();

    AutoPtr<_TestRefCountingObject> trco = NEW<_TestRefCountingObject>();

    assert(trco == _constructed);

    assert(!_destructed);

    trco = nullptr;

    assert(_destructed);

    _recoverState();

    NEW<_TestRefCountingObject>(_NEW__ARGUMENTS);

    assert(_nConstructings == 1U);

    assert(_constructed == _destructed);

    assert(_constructorArguments == Arguments(_NEW__ARGUMENTS));

    _recoverState();

    trco = NEW<_TestRefCountingObject>(_NEW__ARGUMENTS);

    assert(trco == _constructed);

    assert(!_destructed);

    trco = nullptr;

    assert(_destructed);

    _recoverState();
  }

  cout << "\"test-auto-ptr\" passed." << endl;

  return 0;
}

