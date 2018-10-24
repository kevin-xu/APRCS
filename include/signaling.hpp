/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Kevin XU <kevin.xu.1982.02.06@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 *
 * Author: Kevin XU <kevin.xu.1982.02.06@gmail.com>
 *
 */

#ifndef __SIGNALING_HPP
# define __SIGNALING_HPP

# include <cassert>

# include <deque>
# include <map>
# include <type_traits>
# include <utility>



class Signaling {
public:
  template <class S>
  using EIIBOSSVIT = typename std::enable_if<std::is_base_of<Signaling, S>::value, int>::type;

private:
  template <class S, class ... As>
  struct _Slot {
    typedef void (*Type)(S &signaling, As... arguments, void *data);
  };

  template <class S>
  struct _Slot<S, void> {
    typedef void (*Type)(S &signaling, void *data);
  };

public:
  template <class S, class ... As>
  using Slot = typename _Slot<S, As...>::Type;

  template <class S, class ... AsD>
  using Slot2 = void (*)(S &signaling, AsD... argumentsAndData);

private:
  template <class ... As>
  struct _SIGNATURE {
    template <class S, EIIBOSSVIT<S> = 0>
    using SLOT = Slot<S, As...>;
  };

public:
  template <class ... As>
  using SIGNATURE = _SIGNATURE<As...>;

  template <class S, int signal, EIIBOSSVIT<S> = 0>
  struct SIGNALS {};

  struct ConnectionId {
    int signal;

    unsigned subconnectionId;
  };

  template <int signal, class S, class ... AsD>
  static ConnectionId connect(S *self, Slot2<S, AsD...> slot, void *data)
  {
    static_assert(std::is_base_of<Signaling, S>::value, "");

    typedef typename SIGNALS<S, signal>::SIGNATURE _SIGNATURE;

    static_assert(IsInstanceOfSIGNATURE<_SIGNATURE>::value, "");

    static_assert(std::is_same<Slot2<S, AsD...>, typename _SIGNATURE::template SLOT<S>>::value, "");

    DU &dsi = self->_ms2dsi[signal];

    bool empty = dsi.empty();

    unsigned subconnectionId;

    if (empty) {
      if (self->_ms2si.count(signal) == 0)
        self->_ms2si[signal] = 0;

      subconnectionId = self->_ms2si[signal];
    } else
      subconnectionId = dsi.front();

    self->_ms2msi2sd[signal].emplace(subconnectionId, PSPV((Slot0)slot, data));

    if (empty)
      ++self->_ms2si[signal];
    else
      dsi.pop_front();

    ConnectionId connectionId;

    connectionId.signal = signal;

    connectionId.subconnectionId = subconnectionId;

    return connectionId;
  }

  void disconnect(ConnectionId const &connectionId)
  {
    int signal = connectionId.signal;

    unsigned subconnectionId = connectionId.subconnectionId;

    if (_ms2msi2sd.count(signal) == 0)
      return;

    MUPSPV &msi2sd = _ms2msi2sd[signal];

    if (msi2sd.count(subconnectionId) == 0)
      return;

    msi2sd.erase(subconnectionId);

    _ms2dsi[signal].emplace_back(subconnectionId);
  }

  void disconnect(int signal) noexcept
  {
    if (_ms2msi2sd.count(signal) == 0)
      return;

    _ms2msi2sd[signal].clear();

    if (_ms2dsi.count(signal) == 0)
      return;

    _ms2dsi[signal].clear();

    if (_ms2si.count(signal) == 0)
      return;

    _ms2si[signal] = 0;
  }

  void disconnect(void) noexcept
  {
    for (auto i = _ms2si.cbegin(), end = _ms2si.cend(); i != end; ++i)
      disconnect(i->first);
  }

protected:
  Signaling(void) = default;

  Signaling(Signaling const &signaling) = default;

  Signaling(Signaling &&signaling) = default;

  ~Signaling() = default;

  Signaling &operator=(Signaling const &signaling) = default;

  Signaling &operator=(Signaling &&signaling) = default;

  template <int signal, class S, class ... As>
  static void emit(S *self, As... arguments)
  {
    static_assert(std::is_base_of<Signaling, S>::value, "");

    typedef typename SIGNALS<S, signal>::SIGNATURE _SIGNATURE;

    static_assert(IsInstanceOfSIGNATURE<_SIGNATURE>::value, "");

    typedef typename _SIGNATURE::template SLOT<S> _Slot;

    MUPSPV const &msi2sd = self->_ms2msi2sd[signal];

    for (auto i = msi2sd.cbegin(), end = msi2sd.cend(); i != end; ++i) {
      _Slot slot = (_Slot)i->second.first;

      void *data = i->second.second;

      (*slot)(*self, arguments..., data);
    }
  }

private:
  template <class T>
  struct IsInstanceOfSIGNATURE {
    static bool constexpr value = false;
  };

  template <class ... As>
  struct IsInstanceOfSIGNATURE<SIGNATURE<As...>> {
    static bool constexpr value = true;
  };

  typedef std::map<int, unsigned> MIU;

  typedef std::deque<unsigned> DU;
  typedef std::map<int, DU> MIDU;

  typedef void (*Slot0)(...);

  typedef std::pair<Slot0, void *> PSPV;
  typedef std::map<unsigned, PSPV> MUPSPV;
  typedef std::map<int, MUPSPV> MIMUPSPV;

  MIU _ms2si;

  MIDU _ms2dsi;

  MIMUPSPV _ms2msi2sd;
};

#endif
