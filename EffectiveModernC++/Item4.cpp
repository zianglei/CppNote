/**
 * @file Item4.cpp
 * @brief
 * @date 2020/9/11
 */

#include <vector>
#include <string>
#ifndef _MSC_VER
#   include <cxxabi.h>
#include <iostream>

#endif

template <typename T>
class TD;

struct Widget {};

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
    r += own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

template <typename T>
void f(const T& param) {
    std::cout << type_name<T>() << std::endl;
    std::cout << type_name<decltype(param)>() << std::endl;
}

template <typename T>
void wrongF(const T& param) {
    std::cout << "T = " << typeid(T).name() << '\n';
    std::cout << "param = " << typeid(param).name() << '\n';
}

/*
 * - 可以通过编译器报错来获得变量推导类型
 * - 使用typeid(var).name()得到的变量名并不准确，忽略了对应的引用
 * - 可以使用boost::typeindex::type_id_with_cvr获得变量包含引用等修饰值的类型
 */
int main() {
    const int theAnswer = 24;
    auto x = theAnswer;
    auto& y = theAnswer;

//    TD<decltype(x)> xType; // 通过报错信息获得x的类型
//    TD<decltype(y)> yType; // 通过报错信息获得y的类型

    // 通过typeid(x).name获得的类型不准确

    std::vector<Widget> w(1);
    const auto vw = w;
    wrongF(&vw[0]);
    f(&vw[0]);

    // 可以使用boost::typeindex::type_id_with_cvr获取变量的实际类型

    return 0;
}