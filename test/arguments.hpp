/*
 *
 * Author: Kevin XU <kevin.xu.1982.02.06@gmail.com>
 *
 */

#ifndef __ARGUMENTS_HPP
# define __ARGUMENTS_HPP

# include <cstddef>

# include <sstream>
# include <string>
# include <typeinfo>
# include <vector>



namespace Test {

class Arguments;

inline bool operator==(Arguments const &lhs, Arguments const &rhs) noexcept;

class Arguments {
public:
  template <class ... Ts>
  Arguments(Ts... arguments)
  {
    saveArguments0(arguments...);
  }

  template <class ... Ts>
  void operator()(Ts... arguments)
  {
    saveArguments(arguments...);
  }

private:
  std::vector<size_t> _types;

  std::ostringstream _values;

  void saveArguments0(void) noexcept {}

  template <class T, class ... Ts>
  void saveArguments0(T argument0, Ts... arguments)
  {
    _types.emplace_back(typeid(argument0).hash_code());

    _values << argument0 << std::endl;

    saveArguments0(arguments...);
  }

  template <class ... Ts>
  void saveArguments(Ts... arguments)
  {
    _types.clear();

    _values.flush();

    saveArguments0(arguments...);
  }

  friend bool operator==(Arguments const &lhs, Arguments const &rhs) noexcept;
};

inline bool operator==(Arguments const &lhs, Arguments const &rhs) noexcept
{
  if (lhs._types != rhs._types)
    return false;

  if (lhs._values.str() != rhs._values.str())
    return false;

  return true;
}

}

#endif
