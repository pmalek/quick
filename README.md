# quick

![C++](https://img.shields.io/badge/language-C%2B%2B-blue.svg) ![License](https://img.shields.io/badge/license-GPLv3-blue.svg)

`quick` aims to be a small header only library to help black box testing in C++. 
It borrows the idea from Golang's [testing/quick package](https://golang.org/pkg/testing/quick/)

### Idea

Create a predicate where you want to test your struct/class/function/member function and return true on success.
`quick` will randomly generate input data to this predicate and launch it predefined number of iterations or will keep running for predefined time duration.

### Note
`quick` is in very alpha state not even remotely close to being ready for production 

### Example usage:

```cpp
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

int main()
{
    auto f1 = [](const std::string& s, int n){
      const std::string ret = prefix(s, n);
      return ret == s.substr(0, n);
    };
    quick::check(f1, 200ms, "Prefix always has N length - 200ms");

    auto f2 = [](const std::string& s, int n){
      const std::string ret = prefix(s, n);
      return ret == s.substr(0, n);
    };
    quick::check(f2, 100, "Prefix always has N length - 100 iterations");
}
```
