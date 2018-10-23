/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Kevin XU <kevin.xu.1982.02.06@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the “Software”), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
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

#ifndef __AUTO_PTR_HPP
# define __AUTO_PTR_HPP

# include <cassert>
# include <cstddef>

# include <type_traits>

# include "ref-counting.hpp"



template <class RC>
class AutoPtr {
  static_assert(RefCounting::CHECK<RC>::value, "");
public:
  typedef RC ContentType;

  template <class F, class T>
  using EIICPFPTVIT = typename std::enable_if<std::is_convertible<F *, T *>::value, int>::type;

  template <class F, class T>
  using EINICPFPTVIT = typename std::enable_if<!std::is_convertible<F *, T *>::value, int>::type;

  constexpr AutoPtr(void) noexcept: _pointer(nullptr) {}

  constexpr AutoPtr(std::nullptr_t) noexcept: _pointer(nullptr) {}

  AutoPtr(RC *pointer) noexcept
  {
    set(pointer);
  }

  AutoPtr(AutoPtr const &autoPtr) noexcept
  {
    set(autoPtr._pointer);
  }

  template <class RRC, EIICPFPTVIT<RRC, RC> = 0>
  AutoPtr(AutoPtr<RRC> const &autoPtr) noexcept
  {
    set(autoPtr._pointer);
  }

  AutoPtr(AutoPtr &&autoPtr) noexcept
  {
    move0(autoPtr);
  }

  template <class RRC, EIICPFPTVIT<RRC, RC> = 0>
  AutoPtr(AutoPtr<RRC> &&autoPtr) noexcept
  {
    move0(autoPtr);
  }

  ~AutoPtr()
  {
    deref();
  }

  AutoPtr &operator=(std::nullptr_t)
  {
    if (_pointer == nullptr)
      return *this;

    _pointer->deref();

    _pointer = nullptr;

    return *this;
  }

  AutoPtr &operator=(RC *pointer)
  {
    reset(pointer);

    return *this;
  }

  AutoPtr &operator=(AutoPtr const &autoPtr)
  {
    reset(autoPtr._pointer);

    return *this;
  }

  template <class RRC, EIICPFPTVIT<RRC, RC> = 0>
  AutoPtr &operator=(AutoPtr<RRC> const &autoPtr)
  {
    reset(autoPtr._pointer);

    return *this;
  }

  AutoPtr &operator=(AutoPtr &&autoPtr)
  {
    move(autoPtr);

    return *this;
  }

  template <class RRC, EIICPFPTVIT<RRC, RC> = 0>
  AutoPtr &operator=(AutoPtr<RRC> &&autoPtr)
  {
    move(autoPtr);

    return *this;
  }

  AutoPtr &operator+=(int offset)
  {
    reset(_pointer + offset);

    return *this;
  }

  AutoPtr &operator-=(int offset)
  {
    reset(_pointer - offset);

    return *this;
  }

  AutoPtr &operator++()
  {
    return *this += 1;
  }

  AutoPtr operator++(int)
  {
    RC *pointer = _pointer;

    ++*this;

    return pointer;
  }

  AutoPtr &operator--()
  {
    return *this -= 1;
  }

  AutoPtr operator--(int)
  {
    RC *pointer = _pointer;

    --*this;

    return pointer;
  }

  RC *operator->() const noexcept
  {
    return _pointer;
  }

  RC &operator*() const noexcept
  {
    return *_pointer;
  }

  RC &operator[](int index) const noexcept
  {
    return _pointer[index];
  }

  operator RC * () const noexcept
  {
    return _pointer;
  }

  template <class T, EIICPFPTVIT<RC, T> = 0>
  operator T * () const noexcept
  {
    return _pointer;
  }

  template <class T, EINICPFPTVIT<RC, T> = 0>
  explicit operator T * () const noexcept
  {
    return static_cast<T *>(_pointer);
  }

  operator bool () const noexcept
  {
    return !!_pointer;
  }

  void swap(AutoPtr &autoPtr) noexcept
  {
    if (autoPtr._pointer == _pointer)
      return;

    RC *pointer = _pointer;

    _pointer = autoPtr._pointer;

    autoPtr._pointer = pointer;
  }

private:
  RC *_pointer;

  void set(RC *pointer) noexcept
  {
    if (pointer != nullptr)
      pointer->ref();

    _pointer = pointer;
  }

  template <class RRC>
  void move0(AutoPtr<RRC> &autoPtr) noexcept
  {
    _pointer = autoPtr._pointer, autoPtr._pointer = nullptr;
  }

  void deref(void) const
  {
    if (_pointer != nullptr)
      _pointer->deref();
  }

  void reset(RC *pointer)
  {
    if (pointer == _pointer)
      return;

    deref();

    set(pointer);
  }

  template <class RRC>
  void move(AutoPtr<RRC> &autoPtr)
  {
    if (autoPtr._pointer == _pointer)
      return;

    deref();

    move0(autoPtr);
  }

  template <class RC_>
  friend class AutoPtr;

  template <class L, class R>
  friend bool operator==(AutoPtr<L> const &lhs, AutoPtr<R> const &rhs) noexcept;

  template <class L, class R>
  friend bool operator!=(AutoPtr<L> const &lhs, AutoPtr<R> const &rhs) noexcept;

  template <class T>
  friend T *operator+(AutoPtr<T> const &autoPtr, int offset) noexcept;

  template <class T>
  friend T *operator+(int offset, AutoPtr<T> const &autoPtr) noexcept;

  template <class T>
  friend T *operator-(AutoPtr<T> const &autoPtr, int offset) noexcept;
};

template <class L, class R>
inline bool operator==(AutoPtr<L> const &lhs, AutoPtr<R> const &rhs) noexcept
{
  return lhs._pointer == rhs._pointer;
}

template <class L, class R>
inline bool operator!=(AutoPtr<L> const &lhs, AutoPtr<R> const &rhs) noexcept
{
  return lhs._pointer != rhs._pointer;
}

template <class T>
inline T *operator+(AutoPtr<T> const &autoPtr, int offset) noexcept
{
  return autoPtr._pointer + offset;
}

template <class T>
inline T *operator+(int offset, AutoPtr<T> const &autoPtr) noexcept
{
  return offset + autoPtr._pointer;
}

template <class T>
inline T *operator-(AutoPtr<T> const &autoPtr, int offset) noexcept
{
  return autoPtr._pointer - offset;
}

template <class RC, class ... As>
inline AutoPtr<RC> NEW(As... arguments)
{
  RC *refCounting = new RC(arguments...);

  AutoPtr<RC> _refCounting = refCounting;

  refCounting->deref();

  return _refCounting;
}

#endif
