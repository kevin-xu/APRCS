/*
 *
 * Author: Kevin XU <kevin.xu.1982.02.06@gmail.com>
 *
 */

#include <cassert>

#include <iostream>
#include <string>
#include <utility>

#include "../include/auto-ptr.hpp"
#include "../include/ref-counting.hpp"

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

class _TestRefCounting: public RefCounting {
public:
  _TestRefCounting(void) noexcept
  {
    ++_nConstructings;

    _constructed = this;
  }

  template <class ... Ts>
  _TestRefCounting(Ts... arguments)
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
  ~_TestRefCounting() noexcept
  {
    _destructed = this;
  }
};

class _TestRefCountingDerived: public _TestRefCounting {
public:
  _TestRefCountingDerived(void) = default;

protected:
  ~_TestRefCountingDerived() = default;
};

int main(int argc, char const *argv[])
{
  {
    AutoPtr<_TestRefCounting> trc;

    assert(_nConstructings == 0U);

    assert(trc == nullptr);
  }

  {
    AutoPtr<_TestRefCounting> trc = nullptr;

    assert(_nConstructings == 0U);

    assert(trc == nullptr);
  }

  {
    {
      _TestRefCounting *trc = new _TestRefCounting();

      AutoPtr<_TestRefCounting> _trc = trc;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(_trc == trc);

      trc->deref();

      assert(!_destructed);
    }

    assert(_destructed);

    _recoverState();
  }

  {
    {
      _TestRefCounting *trc = new _TestRefCounting();

      AutoPtr<_TestRefCounting> _trc = trc;

      AutoPtr<_TestRefCounting> __trc = _trc;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(__trc == trc);

      trc->deref();

      assert(!_destructed);
    }

    assert(_destructed);

    _recoverState();
  }

  {
    {
      _TestRefCountingDerived *trcd = new _TestRefCountingDerived();

      AutoPtr<_TestRefCountingDerived> _trcd = trcd;

      AutoPtr<_TestRefCounting> trc = _trcd;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc == trcd);

      trcd->deref();

      assert(!_destructed);
    }

    assert(_destructed);

    _recoverState();
  }

  {
    {
      _TestRefCounting *trc = new _TestRefCounting();

      AutoPtr<_TestRefCounting> _trc = trc;

      AutoPtr<_TestRefCounting> __trc = move(_trc);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(__trc == trc);

      trc->deref();

      assert(!_destructed);
    }

    assert(_destructed);

    _recoverState();
  }

  {
    {
      _TestRefCountingDerived *trcd = new _TestRefCountingDerived();

      AutoPtr<_TestRefCountingDerived> _trcd = trcd;

      AutoPtr<_TestRefCounting> trc = move(_trcd);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc == trcd);

      trcd->deref();

      assert(!_destructed);
    }

    assert(_destructed);

    _recoverState();
  }

  {
    {
      _TestRefCounting *trc = new _TestRefCounting();

      AutoPtr<_TestRefCounting> _trc = trc;

      trc->deref();
    }

    assert(_destructed);

    _recoverState();
  }

  {
    AutoPtr<_TestRefCounting> trc;

    trc = nullptr;

    assert(_nConstructings == 0U);

    assert(trc == nullptr);

    _TestRefCounting *_trc = new _TestRefCounting();

    AutoPtr<_TestRefCounting> __trc = _trc;

    _trc->deref();

    __trc = nullptr;

    assert(_nConstructings == 1U);

    assert(_destructed);

    assert(__trc == nullptr);

    _recoverState();
  }

  {
    {
      _TestRefCounting *trc = new _TestRefCounting();

      AutoPtr<_TestRefCounting> _trc;

      _trc = trc;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(_trc == trc);

      trc->deref();

      assert(!_destructed);

      _trc = nullptr;

      assert(_destructed);

      _recoverState();

      trc = new _TestRefCounting();

      AutoPtr<_TestRefCounting> __trc = trc;

      __trc = trc;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(__trc == trc);

      trc->deref();

      assert(!_destructed);

      __trc = nullptr;

      assert(_destructed);

      _recoverState();

      trc = new _TestRefCounting();

      AutoPtr<_TestRefCounting> ___trc = trc;

      trc->deref();

      _TestRefCounting *trc2 = new _TestRefCounting();

      ___trc = trc2;

      assert(_nConstructings == 2U);

      assert(_destructed == trc);

      assert(___trc == trc2);

      trc2->deref();

      assert(_destructed != trc2);

      ___trc = nullptr;

      assert(_destructed == trc2);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCounting> trc1;

      AutoPtr<_TestRefCounting> trc2;

      trc1 = trc2;

      assert(_nConstructings == 0U);

      assert(trc1 == nullptr);

      assert(trc2 == nullptr);

      _TestRefCounting *_trc1 = new _TestRefCounting();

      trc1 = _trc1;

      _trc1->deref();

      trc1 = trc2;

      assert(_nConstructings == 1U);

      assert(_destructed);

      assert(trc1 == nullptr);

      assert(trc2 == nullptr);

      _recoverState();

      _TestRefCounting *_trc2 = new _TestRefCounting();

      trc2 = _trc2;

      trc1 = trc2;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == _trc2);

      assert(trc2 == _trc2);

      _trc2->deref();

      assert(!_destructed);

      trc1 = nullptr;

      assert(!_destructed);

      trc2 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCounting *trc = new _TestRefCounting();

      trc1 = trc;

      trc2 = trc;

      trc1 = trc2;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == trc);

      assert(trc2 == trc);

      trc->deref();

      assert(!_destructed);

      trc1 = nullptr;

      assert(!_destructed);

      trc2 = nullptr;

      assert(_destructed);

      _recoverState();

      _trc1 = new _TestRefCounting();

      trc1 = _trc1;

      _trc1->deref();

      _trc2 = new _TestRefCounting();

      trc2 = _trc2;

      trc1 = trc2;

      assert(_nConstructings == 2U);

      assert(_destructed == _trc1);

      assert(trc1 == _trc2);

      assert(trc2 == _trc2);

      _trc2->deref();

      assert(_destructed != _trc2);

      trc1 = nullptr;

      assert(_destructed != _trc2);

      trc2 = nullptr;

      assert(_destructed == _trc2);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCounting> trc;

      AutoPtr<_TestRefCountingDerived> trcd;

      trc = trcd;

      assert(_nConstructings == 0U);

      assert(trc == nullptr);

      assert(trcd == nullptr);

      _TestRefCounting *_trc = new _TestRefCounting();

      trc = _trc;

      _trc->deref();

      trc = trcd;

      assert(_nConstructings == 1U);

      assert(_destructed);

      assert(trc == nullptr);

      assert(trcd == nullptr);

      _recoverState();

      _TestRefCountingDerived *_trcd = new _TestRefCountingDerived();

      trcd = _trcd;

      trc = trcd;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc == _trcd);

      assert(trcd == _trcd);

      _trcd->deref();

      assert(!_destructed);

      trc = nullptr;

      assert(!_destructed);

      trcd = nullptr;

      assert(_destructed);

      _recoverState();

      _trcd = new _TestRefCountingDerived();

      trcd = _trcd;

      trc = _trcd;

      trc = trcd;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc == _trcd);

      assert(trcd == _trcd);

      _trcd->deref();

      assert(!_destructed);

      trc = nullptr;

      assert(!_destructed);

      trcd = nullptr;

      assert(_destructed);

      _recoverState();

      _trc = new _TestRefCounting();

      trc = _trc;

      _trc->deref();

      _trcd = new _TestRefCountingDerived();

      trcd = _trcd;

      trc = trcd;

      assert(_nConstructings == 2U);

      assert(_destructed == _trc);

      assert(trc == _trcd);

      assert(trcd == _trcd);

      _trcd->deref();

      assert(_destructed != _trcd);

      trc = nullptr;

      assert(_destructed != _trcd);

      trcd = nullptr;

      assert(_destructed == _trcd);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCounting> trc1;

      AutoPtr<_TestRefCounting> trc2;

      trc1 = move(trc2);

      assert(_nConstructings == 0U);

      assert(trc1 == nullptr);

      assert(trc2 == nullptr);

      _TestRefCounting *_trc1 = new _TestRefCounting();

      trc1 = _trc1;

      _trc1->deref();

      trc1 = move(trc2);

      assert(_nConstructings == 1U);

      assert(_destructed);

      assert(trc1 == nullptr);

      assert(trc2 == nullptr);

      _recoverState();

      _TestRefCounting *_trc2 = new _TestRefCounting();

      trc2 = _trc2;

      trc1 = move(trc2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == _trc2);

      _trc2->deref();

      assert(!_destructed);

      assert(trc2 == nullptr);

      trc1 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCounting *trc = new _TestRefCounting();

      trc1 = trc;

      trc2 = trc;

      trc1 = move(trc2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == trc);

      assert(trc2 == trc);

      trc->deref();

      assert(!_destructed);

      trc1 = nullptr;

      assert(!_destructed);

      trc2 = nullptr;

      assert(_destructed);

      _recoverState();

      _trc1 = new _TestRefCounting();

      trc1 = _trc1;

      _trc1->deref();

      _trc2 = new _TestRefCounting();

      trc2 = _trc2;

      trc1 = move(trc2);

      assert(_nConstructings == 2U);

      assert(_destructed == _trc1);

      assert(trc1 == _trc2);

      _trc2->deref();

      assert(_destructed != _trc2);

      assert(trc2 == nullptr);

      trc1 = nullptr;

      assert(_destructed == _trc2);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCounting> trc;

      AutoPtr<_TestRefCountingDerived> trcd;

      trc = move(trcd);

      assert(_nConstructings == 0U);

      assert(trc == nullptr);

      assert(trcd == nullptr);

      _TestRefCounting *_trc = new _TestRefCounting();

      trc = _trc;

      _trc->deref();

      trc = move(trcd);

      assert(_nConstructings == 1U);

      assert(_destructed);

      assert(trc == nullptr);

      assert(trcd == nullptr);

      _recoverState();

      _TestRefCountingDerived *_trcd = new _TestRefCountingDerived();

      trcd = _trcd;

      trc = move(trcd);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc == _trcd);

      _trcd->deref();

      assert(!_destructed);

      assert(trcd == nullptr);

      trc = nullptr;

      assert(_destructed);

      _recoverState();

      _trcd = new _TestRefCountingDerived();

      trcd = _trcd;

      trc = _trcd;

      trc = move(trcd);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc == _trcd);

      assert(trcd == _trcd);

      _trcd->deref();

      assert(!_destructed);

      trc = nullptr;

      assert(!_destructed);

      trcd = nullptr;

      assert(_destructed);

      _recoverState();

      _trc = new _TestRefCounting();

      trc = _trc;

      _trc->deref();

      _trcd = new _TestRefCountingDerived();

      trcd = _trcd;

      trc = move(trcd);

      assert(_nConstructings == 2U);

      assert(_destructed == _trc);

      assert(trc == _trcd);

      _trcd->deref();

      assert(_destructed != _trcd);

      assert(trcd == nullptr);

      trc = nullptr;

      assert(_destructed == _trcd);
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
      _TestRefCounting *trc = new _TestRefCounting();

      AutoPtr<_TestRefCounting> _trc = trc;

      trc->deref();

      _trc->doo();

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(_done == true);

      _trc = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
    {
      _TestRefCounting *trc = new _TestRefCounting();

      AutoPtr<_TestRefCounting> _trc = trc;

      _TestRefCounting &__trc = *_trc;

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(&__trc == trc);

      trc->deref();

      assert(!_destructed);

      _trc = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
  }

  {
    {
      AutoPtr<_TestRefCounting> trc;

      assert((_TestRefCounting *)trc == nullptr);

      assert(_nConstructings == 0U);

      trc = nullptr;

      assert(trc == nullptr);

      assert(_nConstructings == 0U);

      _TestRefCounting *_trc = new _TestRefCounting();

      trc = _trc;

      assert((_TestRefCounting *)trc == _trc);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      _trc->deref();

      assert(!_destructed);

      trc = nullptr;

      assert(_destructed);

      _recoverState();

      _trc = new _TestRefCounting();

      trc = _trc;

      assert(trc == _trc);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      _trc->deref();

      assert(!_destructed);

      trc = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCountingDerived> trcd;

      assert((_TestRefCounting *)trcd == nullptr);

      assert(_nConstructings == 0U);

      trcd = nullptr;

      assert(trcd == (_TestRefCounting *)nullptr);

      assert(_nConstructings == 0U);

      _TestRefCountingDerived *_trcd = new _TestRefCountingDerived();

      trcd = _trcd;

      assert((_TestRefCounting *)trcd == _trcd);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      _trcd->deref();

      assert(!_destructed);

      trcd = nullptr;

      assert(_destructed);

      _recoverState();

      _trcd = new _TestRefCountingDerived();

      trcd = _trcd;

      assert(trcd == (_TestRefCounting *)_trcd);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      _trcd->deref();

      assert(!_destructed);

      trcd = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCounting> trc;

      assert((_TestRefCountingDerived *)trc == nullptr);

      assert(_nConstructings == 0U);

      _TestRefCountingDerived *trcd = new _TestRefCountingDerived();

      trc = trcd;

      assert((_TestRefCountingDerived *)trc == trcd);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      trcd->deref();

      assert(!_destructed);

      trc = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCounting> trc;

      assert(!(bool)trc);

      assert(_nConstructings == 0U);

      trc = nullptr;

      assert(!trc);

      assert(_nConstructings == 0U);

      _TestRefCounting *_trc = new _TestRefCounting();

      trc = _trc;

      _trc->deref();

      assert((bool)trc);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      trc = nullptr;

      assert(_destructed);

      _recoverState();

      _trc = new _TestRefCounting();

      trc = _trc;

      _trc->deref();

      assert(trc);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      trc = nullptr;

      assert(_destructed);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCounting> trc1;

      AutoPtr<_TestRefCounting> trc2;

      trc1.swap(trc2);

      assert(_nConstructings == 0U);

      assert(trc1 == nullptr);

      assert(trc2 == nullptr);

      _TestRefCounting *_trc1 = new _TestRefCounting();

      trc1 = _trc1;

      trc1.swap(trc2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == nullptr);

      assert(trc2 == _trc1);

      _trc1->deref();

      assert(!_destructed);

      trc2 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCounting *_trc2 = new _TestRefCounting();

      trc2 = _trc2;

      trc1.swap(trc2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == _trc2);

      _trc2->deref();

      assert(!_destructed);

      assert(trc2 == nullptr);

      trc1 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCounting *trc = new _TestRefCounting();

      trc1 = trc;

      trc2 = trc;

      trc1.swap(trc2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == trc);

      assert(trc2 == trc);

      trc->deref();

      assert(!_destructed);

      trc1 = nullptr;

      assert(!_destructed);

      trc2 = nullptr;

      assert(_destructed);

      _recoverState();

      _trc1 = new _TestRefCounting();

      trc1 = _trc1;

      _trc2 = new _TestRefCounting();

      trc2 = _trc2;

      trc1.swap(trc2);

      assert(_nConstructings == 2U);

      assert(!_destructed);

      assert(trc1 == _trc2);

      _trc2->deref();

      assert(!_destructed);

      assert(trc2 == _trc1);

      _trc1->deref();

      assert(!_destructed);

      trc1 = nullptr;

      assert(_destructed == _trc2);

      trc2 = nullptr;

      assert(_destructed == _trc1);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCounting> trc1;

      AutoPtr<_TestRefCounting> trc2;

      assert(trc1 == trc2);

      assert(_nConstructings == 0U);

      _TestRefCounting *_trc1 = new _TestRefCounting();

      trc1 = _trc1;

      trc2 = nullptr;

      assert(!(trc1 == trc2));

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == _trc1);

      assert(trc2 == nullptr);

      _trc1->deref();

      assert(!_destructed);

      trc1 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCounting *_trc2 = new _TestRefCounting();

      trc1 = nullptr;

      trc2 = _trc2;

      assert(!(trc1 == trc2));

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == nullptr);

      assert(trc2 == _trc2);

      _trc2->deref();

      assert(!_destructed);

      trc2 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCounting *trc = new _TestRefCounting();

      trc1 = trc;

      trc2 = trc;

      trc->deref();

      assert(trc1 == trc2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      trc1 = nullptr;

      assert(!_destructed);

      trc2 = nullptr;

      assert(_destructed);

      _recoverState();

      _trc1 = new _TestRefCounting();

      trc1 = _trc1;

      _trc1->deref();

      _trc2 = new _TestRefCounting();

      trc2 = _trc2;

      _trc2->deref();

      assert(!(trc1 == trc2));

      assert(_nConstructings == 2U);

      assert(!_destructed);

      trc1 = nullptr;

      assert(_destructed == _trc1);

      trc2 = nullptr;

      assert(_destructed == _trc2);
    }

    _recoverState();
  }

  {
    {
      AutoPtr<_TestRefCounting> trc1;

      AutoPtr<_TestRefCounting> trc2;

      assert(!(trc1 != trc2));

      assert(_nConstructings == 0U);

      _TestRefCounting *_trc1 = new _TestRefCounting();

      trc1 = _trc1;

      trc2 = nullptr;

      assert(trc1 != trc2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == _trc1);

      assert(trc2 == nullptr);

      _trc1->deref();

      assert(!_destructed);

      trc1 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCounting *_trc2 = new _TestRefCounting();

      trc1 = nullptr;

      trc2 = _trc2;

      assert(trc1 != trc2);

      assert(_nConstructings == 1U);

      assert(!_destructed);

      assert(trc1 == nullptr);

      assert(trc2 == _trc2);

      _trc2->deref();

      assert(!_destructed);

      trc2 = nullptr;

      assert(_destructed);

      _recoverState();

      _TestRefCounting *trc = new _TestRefCounting();

      trc1 = trc;

      trc2 = trc;

      trc->deref();

      assert(!(trc1 != trc2));

      assert(_nConstructings == 1U);

      assert(!_destructed);

      trc1 = nullptr;

      assert(!_destructed);

      trc2 = nullptr;

      assert(_destructed);

      _recoverState();

      _trc1 = new _TestRefCounting();

      trc1 = _trc1;

      _trc1->deref();

      _trc2 = new _TestRefCounting();

      trc2 = _trc2;

      _trc2->deref();

      assert(trc1 != trc2);

      assert(_nConstructings == 2U);

      assert(!_destructed);

      trc1 = nullptr;

      assert(_destructed == _trc1);

      trc2 = nullptr;

      assert(_destructed == _trc2);
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
    NEW<_TestRefCounting>();

    assert(_nConstructings == 1U);

    assert(_constructed == _destructed);

    _recoverState();

    AutoPtr<_TestRefCounting> trc = NEW<_TestRefCounting>();

    assert(trc == _constructed);

    assert(!_destructed);

    trc = nullptr;

    assert(_destructed);

    _recoverState();

    NEW<_TestRefCounting>(_NEW__ARGUMENTS);

    assert(_nConstructings == 1U);

    assert(_constructed == _destructed);

    assert(_constructorArguments == Arguments(_NEW__ARGUMENTS));

    _recoverState();

    trc = NEW<_TestRefCounting>(_NEW__ARGUMENTS);

    assert(trc == _constructed);

    assert(!_destructed);

    trc = nullptr;

    assert(_destructed);

    _recoverState();
  }

  cout << "\"test-auto-ptr\" passed." << endl;

  return 0;
}

