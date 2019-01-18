/*
 *
 * Author: Kevin XU <kevin.xu.1982.02.06@gmail.com>
 *
 */

#include <cassert>
#include <cstdlib>

#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <signaling.hpp>

#include "arguments.hpp"
#include "rand.hpp"


#define _PASS_NON_VOID_FIXED__SIGNATURE \
  char, int, float, double, string const &, void *

#define _PASS_NON_VOID_FIXED__ARGUMENTS \
  '?', 25, 0.1f, 0.07, string("xyz"), (void *)nullptr

#define _RAND_MAX 1024



using namespace std;

using namespace Test;

class _TestSignalingObject: public Signaling {
public:
  enum {
    SIGNAL_PASS_VOID,
    SIGNAL_PASS_NON_VOID_FIXED
  };

  _TestSignalingObject(void) = default;

  ~_TestSignalingObject() = default;

  template <int signal, class ... As>
  void notify(As... arguments) noexcept
  {
    emit<signal>(this, arguments...);
  }
};

template <>
struct Signaling::SIGNALS<_TestSignalingObject, _TestSignalingObject::SIGNAL_PASS_VOID> {
  typedef Signaling::SIGNATURE<void> SIGNATURE;
};

template <>
struct Signaling::SIGNALS<_TestSignalingObject, _TestSignalingObject::SIGNAL_PASS_NON_VOID_FIXED> {
  typedef Signaling::SIGNATURE<_PASS_NON_VOID_FIXED__SIGNATURE> SIGNATURE;
};

static unsigned _nPassingVoid = 0U;

static vector<_TestSignalingObject *> _tsosPassingVoid;

static vector<void *> _vdataPassingVoid;

static unsigned _nDetachingDataPassingVoid = 0U;

static vector<void *> _vdetachedDataPassingVoid;

static unsigned _nPassingNonVoidFixed = 0U;

static vector<_TestSignalingObject *> _tsosPassingNonVoidFixed;

static vector<Arguments> _argumentssPassingNonVoidFixed;

static vector<void *> _vdataPassingNonVoidFixed;

static unsigned _nDetachingDataPassingNonVoidFixed = 0U;

static vector<void *> _vdetachedDataPassingNonVoidFixed;

static void _recoverState(void) noexcept
{
  _nPassingVoid = 0U;

  _tsosPassingVoid.clear();

  _vdataPassingVoid.clear();

  _nDetachingDataPassingVoid = 0U;

  _vdetachedDataPassingVoid.clear();

  _nPassingNonVoidFixed = 0U;

  _tsosPassingNonVoidFixed.clear();

  _argumentssPassingNonVoidFixed.clear();

  _vdataPassingNonVoidFixed.clear();

  _nDetachingDataPassingNonVoidFixed = 0U;

  _vdetachedDataPassingNonVoidFixed.clear();
}

static void _detechDataPassingVoid(void *data) noexcept
{
  try {
    ++_nDetachingDataPassingVoid;

    _vdetachedDataPassingVoid.emplace_back(data);
  } catch (...) {
    abort();
  }
}

static void _handlePassVoid(_TestSignalingObject &tso, void *data) noexcept
{
  try {
    ++_nPassingVoid;

    _tsosPassingVoid.emplace_back(&tso);

    _vdataPassingVoid.emplace_back(data);
  } catch (...) {
    abort();
  }
}

static void _detechDataPassingNonVoidFixed(void *data) noexcept
{
  try {
    ++_nDetachingDataPassingNonVoidFixed;

    _vdetachedDataPassingNonVoidFixed.emplace_back(data);
  } catch (...) {
    abort();
  }
}

template <class ... As>
static void _handlePassNonVoidFixed(_TestSignalingObject &tso, As... arguments, void *data) noexcept
{
  try {
    ++_nPassingNonVoidFixed;

    _tsosPassingNonVoidFixed.emplace_back(&tso);

    _argumentssPassingNonVoidFixed.emplace_back(arguments...);

    _vdataPassingNonVoidFixed.emplace_back(data);
  } catch (...) {
    abort();
  }
}

int main(int argc, char const *argv[])
{
  _TestSignalingObject tso;

  char *data = new char;

  unsigned n = rand(_RAND_MAX);

  vector<Signaling::ConnectionId> cis(n);

  for (unsigned i = 0U; i < n; ++i)
    cis[i] = _TestSignalingObject::connect<_TestSignalingObject::SIGNAL_PASS_VOID>(
        &tso,
        &_handlePassVoid,
        data,
        &_detechDataPassingVoid);

  tso.notify<_TestSignalingObject::SIGNAL_PASS_VOID>();

  assert(_nPassingVoid == n);

  for (auto i = _tsosPassingVoid.begin(), end = _tsosPassingVoid.end(); i != end; ++i)
    assert(*i == &tso);

  for (auto i = _vdataPassingVoid.begin(), end = _vdataPassingVoid.end(); i != end; ++i)
    assert(*i == data);

  _recoverState();

  set<unsigned> si;

  for (unsigned i = 0U, end = rand(n); i < end; ++i)
    si.emplace(rand(n));

  for (auto i = si.begin(), end = si.end(); i != end; ++i)
    tso.disconnect(cis[*i]);

  size_t sis = si.size();

  assert(_nDetachingDataPassingVoid == sis);

  for (auto i = _vdetachedDataPassingVoid.begin(), end = _vdetachedDataPassingVoid.end();
      i != end;
      ++i)
    assert(*i == data);

  tso.notify<_TestSignalingObject::SIGNAL_PASS_VOID>();

  assert(_nPassingVoid == n - sis);

  for (auto i = _tsosPassingVoid.begin(), end = _tsosPassingVoid.end(); i != end; ++i)
    assert(*i == &tso);

  for (auto i = _vdataPassingVoid.begin(), end = _vdataPassingVoid.end(); i != end; ++i)
    assert(*i == data);

  _recoverState();

  tso.disconnect(_TestSignalingObject::SIGNAL_PASS_VOID);

  assert(_nDetachingDataPassingVoid == n - sis);

  for (auto i = _vdetachedDataPassingVoid.begin(), end = _vdetachedDataPassingVoid.end();
      i != end;
      ++i)
    assert(*i == data);

  tso.notify<_TestSignalingObject::SIGNAL_PASS_VOID>();

  assert(_nPassingVoid == 0U);

  _recoverState();

  Arguments arguments(_PASS_NON_VOID_FIXED__ARGUMENTS);

  n = rand(_RAND_MAX);

  cis.resize(n);

  for (unsigned i = 0U; i < n; ++i)
    cis[i] = _TestSignalingObject::connect<_TestSignalingObject::SIGNAL_PASS_NON_VOID_FIXED>(
        &tso,
        &_handlePassNonVoidFixed<_PASS_NON_VOID_FIXED__SIGNATURE>,
        data,
        &_detechDataPassingNonVoidFixed);

  tso.notify<_TestSignalingObject::SIGNAL_PASS_NON_VOID_FIXED>(_PASS_NON_VOID_FIXED__ARGUMENTS);

  assert(_nPassingNonVoidFixed == n);

  for (auto i = _tsosPassingNonVoidFixed.begin(), end = _tsosPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == &tso);

  for (auto i = _argumentssPassingNonVoidFixed.begin(), end = _argumentssPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == arguments);

  for (auto i = _vdataPassingNonVoidFixed.begin(), end = _vdataPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == data);

  _recoverState();

  si.clear();

  for (unsigned i = 0U, end = rand(n); i < end; ++i)
    si.emplace(rand(n));

  for (auto i = si.begin(), end = si.end(); i != end; ++i)
    tso.disconnect(cis[*i]);

  sis = si.size();

  assert(_nDetachingDataPassingNonVoidFixed == sis);

  for (auto i = _vdetachedDataPassingNonVoidFixed.begin(),
      end = _vdetachedDataPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == data);

  tso.notify<_TestSignalingObject::SIGNAL_PASS_NON_VOID_FIXED>(_PASS_NON_VOID_FIXED__ARGUMENTS);

  assert(_nPassingNonVoidFixed == n - sis);

  for (auto i = _tsosPassingNonVoidFixed.begin(), end = _tsosPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == &tso);

  for (auto i = _argumentssPassingNonVoidFixed.begin(), end = _argumentssPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == arguments);

  for (auto i = _vdataPassingNonVoidFixed.begin(), end = _vdataPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == data);

  _recoverState();

  tso.disconnect(_TestSignalingObject::SIGNAL_PASS_NON_VOID_FIXED);

  assert(_nDetachingDataPassingNonVoidFixed == n - sis);

  for (auto i = _vdetachedDataPassingNonVoidFixed.begin(),
      end = _vdetachedDataPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == data);

  tso.notify<_TestSignalingObject::SIGNAL_PASS_NON_VOID_FIXED>(_PASS_NON_VOID_FIXED__ARGUMENTS);

  assert(_nPassingNonVoidFixed == 0U);

  _recoverState();

  unsigned n1 = rand(_RAND_MAX);

  vector<Signaling::ConnectionId> cis1(n1);

  for (unsigned i = 0U; i < n1; ++i)
    cis1[i] = _TestSignalingObject::connect<_TestSignalingObject::SIGNAL_PASS_VOID>(
        &tso,
        &_handlePassVoid,
        data,
        &_detechDataPassingVoid);

  unsigned n2 = rand(_RAND_MAX);

  vector<Signaling::ConnectionId> cis2(n2);

  for (unsigned i = 0U; i < n2; ++i)
    cis2[i] = _TestSignalingObject::connect<_TestSignalingObject::SIGNAL_PASS_NON_VOID_FIXED>(
        &tso,
        &_handlePassNonVoidFixed<_PASS_NON_VOID_FIXED__SIGNATURE>,
        data,
        &_detechDataPassingNonVoidFixed);

  tso.notify<_TestSignalingObject::SIGNAL_PASS_VOID>();

  tso.notify<_TestSignalingObject::SIGNAL_PASS_NON_VOID_FIXED>(_PASS_NON_VOID_FIXED__ARGUMENTS);

  assert(_nPassingVoid == n1);

  for (auto i = _tsosPassingVoid.begin(), end = _tsosPassingVoid.end(); i != end; ++i)
    assert(*i == &tso);

  for (auto i = _vdataPassingVoid.begin(), end = _vdataPassingVoid.end(); i != end; ++i)
    assert(*i == data);

  assert(_nPassingNonVoidFixed == n2);

  for (auto i = _tsosPassingNonVoidFixed.begin(), end = _tsosPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == &tso);

  for (auto i = _argumentssPassingNonVoidFixed.begin(), end = _argumentssPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == arguments);

  for (auto i = _vdataPassingNonVoidFixed.begin(), end = _vdataPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == data);

  _recoverState();

  tso.disconnect();

  assert(_nDetachingDataPassingVoid == n1);

  for (auto i = _vdetachedDataPassingVoid.begin(), end = _vdetachedDataPassingVoid.end();
      i != end;
      ++i)
    assert(*i == data);

  assert(_nDetachingDataPassingNonVoidFixed == n2);

  for (auto i = _vdetachedDataPassingNonVoidFixed.begin(),
      end = _vdetachedDataPassingNonVoidFixed.end();
      i != end;
      ++i)
    assert(*i == data);

  tso.notify<_TestSignalingObject::SIGNAL_PASS_VOID>();

  tso.notify<_TestSignalingObject::SIGNAL_PASS_NON_VOID_FIXED>(_PASS_NON_VOID_FIXED__ARGUMENTS);

  assert(_nPassingVoid == 0U);

  assert(_nPassingNonVoidFixed == 0U);

  delete data;

  return 0;
}

