/**
 * @file Item3.cpp
 * @brief decltype的使用
 * @date 2020/9/10
 */

#include <type_traits>
#include <iostream>
#include <vector>

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

/**
 * 返回值型别尾序语法
 * 尾序返回值的好处是可以直接使用形参
 */
template<typename Container, typename Index>
auto authAndAccess(Container& c, Index i) -> decltype(c[i]) {
    return c[i];
}

/*
 * 在c++14中，可以直接使用auto进行函数返回值的型别推导
 * 但是根据auto的型别推导规则，返回型别被推导为int，当调用此函数的时候函数的返回值就为右值，无法修改
 *
 * 此函数还无法接受右值引用
 */
template<typename Container, typename Index>
auto authAndAccess14(Container& c, Index i) {
    return c[i];
}

/**
 * C++14
 * 因此要使用decltype(auto)返回int&
 * decltype(auto)的意思是使用auto指定欲实施推导的类型，而推导过程采用的是decltype的规则
 */
template<typename Container, typename Index>
decltype(auto) authAndAccessDecltype14(Container& c, Index i) {
    return c[i];
}

/**
 * 然后使用万能引用，从而使模板函数接受右值，并且对于万能引用要使用std::forward
 */
template <typename Container, typename Index>
decltype(auto) authAndAccessRV14(Container&& c, Index i) {
    return std::forward<Container>(c)[i];
}

/*
 * C++11版本，此处的auto对类型推断并没有作用，主要是表示使用了返回值型别尾序用法
 */
template <typename Container, typename Index>
auto authAndAccessRV11(Container&& c, Index i) -> decltype(std::forward<Container>(c)[i])
{
    return std::forward<Container>(c)[i];
}

/**
 * - 可以使用decltype得到变量的直接类型
 * - 可以在c++11的"返回值型别尾序语法"中使用形参来指定函数返回值的类型，但是在c++14中，直接使用auto（即值传递）会导致出现一些错误（右值引用）
 * - 因此可以使用decltype(auto)来指定使用decltype的型别推导规则
 * - decltype在只传入变量的名的时候会显示变量的具体类型，如果传入的是表达式，则会得到变量的引用类型
 */
int main() {
    /* decltype返回给定的名字或者表达式的确切类型 */
    const int i = 0;
    std::cout << type_name<decltype(i)>() << std::endl;

    std::vector<int> d(2, 0);
    authAndAccess(d, 1) = 10;   // 不会报错，返回的是int&
    std::cout << type_name<decltype(authAndAccess(d, 1))>() << std::endl;
    // authAndAccess14(d, 5) = 10; // 报错：因为auto推导返回的是int，而函数的返回值是作为右值，因此无法进行赋值
    std::cout << type_name<decltype(authAndAccess14(d, 1))>() << std::endl;

    /*
     * Decltype的特殊情况，如果传入的仅仅是变量名称，则返回的是变量的确切类型，
     * 但是如果传入的是复杂的左值表达式的话，返回的就是引用类型
     * 在C++14中，如果返回的是(x)，而返回值类型使用的是decltype(auto)的话，就有可能返回一个临时变量的引用，可能会触发未定义行为
     */
    int x = 0;
    std::cout << type_name<decltype(x)>() << std::endl;
    std::cout << type_name<decltype((x))>() << std::endl;
    std::cout << type_name<decltype(((x)))>() << std::endl;

}