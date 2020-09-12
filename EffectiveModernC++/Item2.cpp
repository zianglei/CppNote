/**
 * @file Item2.cpp
 * @brief auto型别推导
 * @date 2020/9/9
 */
#include <type_traits>
#include <iostream>

#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <string>

/* Reference from https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c */
template <class T>
std::string
type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
            (
#ifndef _MSC_VER
            abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                nullptr, nullptr),
#else
            nullptr,
#endif
            std::free
    );
    std::string r;
    if (std::is_const<TR>::value)
        r += "const ";
    r += own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

/*
 * 对于auto来说，其型别推导方式和模板函数型别推导方式相同，除了多了一个std::initializer_list<T>类型
 * 但是如果auto作为lambda表达式的返回值类型推导，则只具有模板类型推导的规则，不支持std::initializer_list
 */

int main() {
    auto x = 27; // 类似Item1中的按值类型推导
    std::cout << "x: " << type_name<decltype(x)>() << std::endl;

    const auto cx = x; // cx被推导为const int, T为int
    std::cout << "cx: " << type_name<decltype(cx)>() << std::endl;

    const auto& rx = x; // 类似Item1中的引用模板函数，但不是万能引用
    std::cout << "rx: " << type_name<decltype(rx)>() << std::endl;

    auto&& uref1 = x; // uref1
    std::cout << "uref1: " << type_name<decltype(uref1)>() << std::endl;

    auto&& uref2 = cx;
    std::cout << "uref2: " << type_name<decltype(uref2)>() << std::endl;

    auto&& uref3 = 27;
    std::cout << "uref3: " << type_name<decltype(uref3)>() << std::endl;

    const char name[] = "R. N. Briggs";
    auto arr1 = name;
    std::cout << "arr1: " << type_name<decltype(arr1)>() << std::endl;

    auto& arr2 = name;
    std::cout << "arr2: " << type_name<decltype(arr2)>() << std::endl;

    // 对于使用初始化列表初始化的变量，发生两次变量类型推导，首先是推导x3的类型，为std::initializer_list，但是由于std::initializer_list
    // 是一个模板，因此要再次推导模板里的类型T
    auto x3 = {27};
    std::cout << "x3: " << type_name<decltype(x3)>() << std::endl;

    // c++14里面允许对函数的返回值或者lambda里的形参类型使用auto声明来进行推导，但是此处的auto只是相当于模板类型推导而不是auto型别推导，
    // 因此如果返回的是初始化列表的话，就无法推导其类型
}