/*
 *
 * Author: Kevin XU <kevin.xu.1982.02.06@gmail.com>
 *
 */

#ifndef __RAND_HPP
# define __RAND_HPP

#include <cstdlib>



namespace Test {

inline int rand(int max) noexcept
{
  return std::rand() % max;
}

}

#endif
