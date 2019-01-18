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
# include <stdexcept>
# include <tuple>
# include <type_traits>
# include <utility>



class Signaling {
public:
  template <class S>
  using EIIBOSSVIT = typename std::enable_if<std::is_base_of<Signaling, S>::value, int>::type;

  using Slot0 = void (*)(...) noexcept;

private:
  template <class S, class ... As>
  struct _Slot {
    using Type = void (*)(S &signaling, As... arguments, void *data) noexcept;
  };

  template <class S>
  struct _Slot<S, void> {
    using Type = void (*)(S &signaling, void *data) noexcept;
  };

public:
  template <class S, class ... As>
  using Slot = typename _Slot<S, As...>::Type;

  template <class S, class ... Ts>
  using Slot2 = void (*)(S &signaling, Ts... values) noexcept;

  using DetachData = void (*)(void *data) noexcept;

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
  static ConnectionId connect(
      S *self,
      Slot2<S, AsD...> slot,
      void *data,
      DetachData detachData = nullptr)
  {
    static_assert(!std::is_const<S>::value, "");

    static_assert(std::is_base_of<Signaling, S>::value, "");

    typedef typename SIGNALS<S, signal>::SIGNATURE _SIGNATURE;

    static_assert(IsInstanceOfSIGNATURE<_SIGNATURE>::value, "");

    static_assert(std::is_same<Slot2<S, AsD...>, typename _SIGNATURE::template SLOT<S>>::value, "");

    return static_cast<Signaling *>(self)->connect(signal, (Slot0)slot, data, detachData);
  }

  template <int signal, class S, class ... As>
  static ConnectionId connect(S *self, Slot2<S, As...> slot)
  {
    static_assert(!std::is_const<S>::value, "");

    static_assert(std::is_base_of<Signaling, S>::value, "");

    typedef typename SIGNALS<S, signal>::SIGNATURE _SIGNATURE;

    static_assert(IsInstanceOfSIGNATURE<_SIGNATURE>::value, "");

    static_assert(std::is_same<Slot<S, As...>, typename _SIGNATURE::template SLOT<S>>::value, "");

    return static_cast<Signaling *>(self)->connect(signal, (Slot0)slot, nullptr, nullptr);
  }

  void disconnect(ConnectionId const &connectionId)
  {
    int signal = connectionId.signal;

    unsigned subconnectionId = connectionId.subconnectionId;

    auto ismsi2sddd = _ms2msi2sddd.find(signal);

    if (ismsi2sddd == _ms2msi2sddd.end())
      return;

    MUTSPVDD &msi2sddd = ismsi2sddd->second;

    auto isisddd = msi2sddd.find(subconnectionId);

    if (isisddd == msi2sddd.end())
      return;

    _ms2dsi[signal].emplace_back(subconnectionId);

    TSPVDD const &sddd = isisddd->second;

    void *data = std::get<1UL>(sddd);

    DetachData detachData = std::get<2UL>(sddd);

    if (detachData != nullptr)
      (*detachData)(data);

    msi2sddd.erase(subconnectionId);
  }

  void disconnect(int signal) noexcept
  {
    auto ismsi2sddd = _ms2msi2sddd.find(signal);

    if (ismsi2sddd == _ms2msi2sddd.end())
      return;

    disconnect(*ismsi2sddd);
  }

  void disconnect(void) noexcept
  {
    if (_ms2msi2sddd.empty())
      return;

    for (auto i = _ms2msi2sddd.begin(), end = _ms2msi2sddd.end(); i != end; ++i)
      disconnect(*i);
  }

protected:
  Signaling(void) = default;

  Signaling(Signaling const &signaling) = default;

  Signaling(Signaling &&signaling) = default;

  ~Signaling() = default;

  Signaling &operator=(Signaling const &signaling) = default;

  Signaling &operator=(Signaling &&signaling) = default;

  template <int signal, class S, class ... As>
  static void emit(S *self, As... arguments) noexcept
  {
    static_assert(!std::is_const<S>::value, "");

    static_assert(std::is_base_of<Signaling, S>::value, "");

    typedef typename SIGNALS<S, signal>::SIGNATURE _SIGNATURE;

    static_assert(IsInstanceOfSIGNATURE<_SIGNATURE>::value, "");

    typedef typename _SIGNATURE::template SLOT<S> _Slot;

    Signaling *_self = static_cast<Signaling *>(self);

    auto ismsi2sddd = _self->_ms2msi2sddd.find(signal);

    if (ismsi2sddd == _self->_ms2msi2sddd.end())
      return;

    MUTSPVDD const &msi2sddd = ismsi2sddd->second;

    if (msi2sddd.empty())
      return;

    for (auto i = msi2sddd.cbegin(), end = msi2sddd.cend(); i != end; ++i) {
      TSPVDD const &sddd = i->second;

      _Slot slot = (_Slot)std::get<0UL>(sddd);

      void *data = std::get<1UL>(sddd);

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

  typedef std::tuple<Slot0, void *, DetachData> TSPVDD;
  typedef std::map<unsigned, TSPVDD> MUTSPVDD;
  typedef std::map<int, MUTSPVDD> MIMUTSPVDD;

  MIU _ms2si;

  MIDU _ms2dsi;

  MIMUTSPVDD _ms2msi2sddd;

  ConnectionId connect(int signal, Slot0 slot, void *data, DetachData detachData)
  {
    if (slot == nullptr)
      throw std::runtime_error("");

    if (_ms2si.count(signal) == 0UL)
      _ms2si[signal] = 0U;

    auto isdsi = _ms2dsi.find(signal);

    bool empty = isdsi == _ms2dsi.end() ? true : isdsi->second.empty();

    unsigned subconnectionId;

    if (empty)
      subconnectionId = _ms2si[signal];
    else
      subconnectionId = isdsi->second.front();

    _ms2msi2sddd[signal].emplace(subconnectionId, TSPVDD(slot, data, detachData));

    if (empty)
      ++_ms2si[signal];
    else
      isdsi->second.pop_front();

    return {signal, subconnectionId};
  }

  void disconnect(MIMUTSPVDD::value_type &smsi2sddd) noexcept
  {
    int signal = smsi2sddd.first;

    MUTSPVDD &msi2sddd = smsi2sddd.second;

    if (msi2sddd.empty())
      return;

    for (auto i = msi2sddd.cbegin(), end = msi2sddd.cend(); i != end; ++i) {
      TSPVDD const &sddd = i->second;

      void *data = std::get<1UL>(sddd);

      DetachData detachData = std::get<2UL>(sddd);

      if (detachData != nullptr)
        (*detachData)(data);
    }

    msi2sddd.clear();

    auto isdsi = _ms2dsi.find(signal);

    if (isdsi != _ms2dsi.end())
      isdsi->second.clear();

    _ms2si[signal] = 0U;
  }
};

#endif
