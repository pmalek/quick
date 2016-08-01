#include <iostream>
#include <string>

#include "quick.hpp"

using namespace std::chrono_literals;

std::string prefix(const std::string& s, int n)
{
  static int counter = 0;
  ++counter;
  return s.substr(0, n);
}

std::string identical(const std::string& s)
{
  return s;
}

int main()
{
  {
    auto f = [](const std::string& s, int n)
    {
      const std::string ret = prefix(s, n);
      return ret == s.substr(0, n);
    };
    quick::check(f, 200ms, "Prefix always has N length");
  }
  {
    auto f = [](const std::string& s, int n)
    {
      const std::string ret = prefix(s, n);
      return ret == s.substr(0, n);
    };
    quick::check(f, 100, "Prefix always has N length");
  }
}
