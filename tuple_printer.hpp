#ifndef TUPLE_PRINTER_HPP
#define TUPLE_PRINTER_HPP

#include <iostream>
#include <tuple>
#include <sstream>

namespace quick_impl
{
template <class Tuple, std::size_t... Is>
constexpr void print_tuple(std::ostream& os, Tuple const& t, std::index_sequence<Is...>)
{
  using swallow = int[];
  (void)swallow{0, (void(os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), 0)...};
}

}  // quick_impl::

template <class... Args>
std::ostream& operator<<(std::ostream& os, std::tuple<Args...> const& t)
{
  os << '(';
  quick_impl::print_tuple(os, t, std::index_sequence_for<Args...>{});
  os << ')';
  return os;
}

template <typename... Types>
std::string operator+(const std::string& s, const std::tuple<Types...>& value)
{
  std::ostringstream ss;
  ss << value;
  return s + ss.str();
}

#endif  // TUPLE_PRINTER_HPP
