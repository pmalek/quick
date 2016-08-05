#ifndef QUICK_HPP
#define QUICK_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <functional>
#include <utility>

#include "chrono_io.h"

#include "tuple_printer.hpp"

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"

namespace quick_impl
{
constexpr int maxlen = 128;

const std::string default_chars = "abcdefghijklmnaoqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
std::uniform_int_distribution<size_t> dist{0, default_chars.length() - 1};

std::mt19937_64 gen{std::random_device()()};
std::uniform_int_distribution<size_t> dist_len{0, maxlen};
std::uniform_int_distribution<int> dist_int{0, std::numeric_limits<int>::max()};
std::bernoulli_distribution dist_bern;

template <class = void, std::size_t... Is>
static constexpr auto tupler(std::index_sequence<Is...>)
{
  return [](auto&& f)
  {
    return std::make_tuple(f(std::integral_constant<std::size_t, Is>{})...);
  };
}

template <std::size_t N>
static constexpr auto tupler()
{
  return tupler(std::make_index_sequence<N>{});
}

namespace detail
{

template<typename F, typename Tup, size_t ...I>
auto call( F&& f, Tup&& t, std::index_sequence<I...> ) {
    return std::forward<F>(f) ( std::get<I>( std::forward<Tup>(t) )... );
}

}  // namespace detail

template <typename T>
struct function_traits : function_traits<decltype(&T::operator())>
{
};  // For generic types, directly use the result of the signature of its 'operator()'

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>  // we specialize for pointers to member function
{
  static constexpr auto arity = sizeof...(Args);  // arity is the number of arguments.

  using result_type = ReturnType;

  using all_args = std::tuple<Args...>;

  template <size_t i>
  using arg = std::tuple_element_t<i, all_args>;
};

template<typename F, typename Tup >
auto call( F&& f, Tup&& t ) {
    using T = std::decay_t<Tup>;

    constexpr auto size = std::tuple_size<T>(); // N3545 for the use of operator().
    using indicies = std::make_index_sequence<size>;

    return detail::call( std::forward<F>(f), std::forward<Tup>(t), indicies{} );
}

}  // namespace quick_impl

struct quick
{
  template <typename F, typename Rep, typename Period>
  static void check(F f, std::chrono::duration<Rep, Period> duration, const std::string& test_name)
  {
    using namespace date;
    std::cout << "****************************************************************\n";
    std::cout << "STARTING TEST: \"" << test_name << "\"\n\tFOR " << duration << '\n';
    auto stop = std::chrono::steady_clock::now() + duration;
    int count = 0;

    while(std::chrono::steady_clock::now() < stop)
    {
      if(!check_impl(f, count)) return;
      ++count;
    }
    std::cout << GREEN "TEST PASSED AFTER " << count << " ITERATIONS" RESET "\n";
    std::cout << "****************************************************************\n";
  }

  template <typename F, typename Integer>
  static void check(F f, Integer n, const std::string& test_name)
  {
    std::cout << "****************************************************************\n";
    std::cout << "STARTING TEST: \"" << test_name << "\"\n\tWITH " << n << " ITERATIONS\n";
    int count = 0;

    while(count < n)
    {
      if(!check_impl(f, count)) return;
      ++count;
    }
    std::cout << GREEN "TEST PASSED AFTER " << count << " ITERATIONS" RESET "\n";
    std::cout << "****************************************************************\n";
  }

private:
  template <typename F, typename Integer>
  static inline bool check_impl(F f, Integer count)
  {
    constexpr int arity = quick_impl::function_traits<F>::arity;
    auto tuple_creator = quick_impl::tupler<arity>();

    auto random_tuple =
        tuple_creator([&](auto i)
                      {
                        return random<ClearType<typename quick_impl::function_traits<F>::template arg<i>>>();
                      });

    if(!quick_impl::call(f, random_tuple))
    {
      std::cout << RED "TEST FAILED ON ITERATION " << count + 1 << '\n';
      std::cout << "\tFAILED PARAMS:\t" << random_tuple << RESET "\n";
      return false;
    }
    return true;
  }

  template <typename T>
  static T random();

  template <typename F>
  using ClearType = std::remove_cv_t<std::remove_reference_t<F>>;
};

template <>
std::string quick::random<std::string>()
{
  std::string ret(quick_impl::dist_len(quick_impl::gen), ' ');
  for (auto& c :ret) {
    c = quick_impl::default_chars[quick_impl::dist(quick_impl::gen)];
  }
  return ret;
}

template <>
int quick::random<int>()
{
  return quick_impl::dist_int(quick_impl::gen);
}

template <>
bool quick::random<bool>()
{
  return quick_impl::dist_bern(quick_impl::gen);
}

#endif  // QUICK_HPP
