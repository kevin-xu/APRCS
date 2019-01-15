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

#ifndef __REF_COUNTING_HPP
# define __REF_COUNTING_HPP

# include <cstddef>

# include <new>
# include <type_traits>



class RefCounting {
private:
  template <class T>
  struct _CHECK {
    static bool constexpr value =
      std::is_base_of<RefCounting, T>::value && !std::is_destructible<T>::value;
  };

public:
  template <class T>
  using CHECK = _CHECK<T>;

  void ref(void) const noexcept
  {
    ++_count;
  }

  template <class T>
  T *ref(void) noexcept
  {
    ref();

    return static_cast<T *>(this);
  }

  template <class T>
  T const *ref(void) const noexcept
  {
    ref();

    return static_cast<T const *>(this);
  }

  void deref(void) const
  {
    if (--_count == 0)
      delete this;
  }

protected:
  constexpr RefCounting(void) noexcept: _count(1) {}

  virtual ~RefCounting() = default;

private:
  mutable unsigned _count;

  static void *operator new [](std::size_t size) = delete;
  static void operator delete [](void *pointer) noexcept = delete;

  static void *operator new [](std::size_t size, std::nothrow_t const &nothrow) noexcept = delete;
  static void operator delete [](void *pointer, std::nothrow_t const &nothrow) noexcept = delete;

  static void *operator new [](std::size_t size, void *pointer) noexcept = delete;
  static void operator delete [](void *pointer, void *pointer2) noexcept = delete;

  RefCounting(RefCounting const &refCounting) = delete;

  RefCounting(RefCounting &&refCounting) = delete;

  RefCounting &operator=(RefCounting const &refCounting) = delete;

  RefCounting &operator=(RefCounting &&refCounting) = delete;
};

#endif
