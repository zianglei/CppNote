/**
 * @file Item1.cpp
 * @brief 理解模板型别推导
 * @date 2020/9/8
 */

#include <type_traits>
#include <iostream>

#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <cstdlib>

/**
 * 要点：
 * - 对正常的模板类型进行推导的时候，具有引用类型的形参会被当作非引用形参处理
 * - 对万能引用形参进行推导的时候，左值实参会被推导为带引用的类型，右值则正常推导
 * - 按值传递的形参，推导时实参的const/volatile属性会被删除
 * - 对于数组和函数来说，传入按值传递的实参会被退化为指针，而传入引用形参会被推导为引用类型（数组的长度信息会被保留）
 */

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
 * 对于
 * template <typename T>
 * void f(ParamType param);
 * 形式的模板函数来说，ParamType的推导类型不仅与T有关，还与ParamType的形式有关
 */

template <typename T>
void NoConstF(T& param) {
    std::cout << "T: " << type_name<T>() << "; ParamType: " << type_name<decltype(param)>() << std::endl;
}

template <typename T>
void ConstF(const T& param) {
    std::cout << "T: " << type_name<T>() << "; ParamType: " << type_name<decltype(param)>() << std::endl;
}

template <typename T>
void PointerF(T* param) {
    std::cout << "T: " << type_name<T>() << "; ParamType: " << type_name<decltype(param)>() << std::endl;
}

template <typename T>
void UniversalF(T&& param) {
    std::cout << "T: " << type_name<T>() << "; ParamType: " << type_name<decltype(param)>() << std::endl;
}

template <typename T>
void TypeOnlyF(T param) {
    std::cout << "T: " << type_name<T>() << "; ParamType: " << type_name<decltype(param)>() << std::endl;
}

/**
 * 对于模板参数类型是指针或者引用的情况，如果传入的实参具有引用属性，则将引用去除，再决定T的类型
 * 如果ParamType带有const类型，则T再推导为const int就没有意义了，因为常量性已经满足，所以T一律为int
 */
void ParamTypeIsAPointerOrRefer() {
    std::cout << "<<<< ParamType is a pointer or reference: " << std::endl;
    std::cout << "<<<<<<< ParamType doesn't have const: " << std::endl;
    int x = 27;
    const int cx = x;
    const int &rx = x;
    std::cout << "x: " << std::endl;
    NoConstF(x);
    std::cout << "cx: " << std::endl;
    NoConstF(cx);
    std::cout << "rx: " << std::endl;
    NoConstF(rx);

    std::cout << "<<<<<<< ParamType has const: " << std::endl;
    std::cout << "x: " << std::endl;
    ConstF(x);
    std::cout << "cx: " << std::endl;
    ConstF(cx);
    std::cout << "rx: " << std::endl;
    ConstF(rx);

    std::cout << "<<<<<<< ParamType is a pointer" << std::endl;
    const int* p = &x;
    std::cout << "&x: " << std::endl;
    PointerF(&x);
    std::cout << "p: " << std::endl;
    PointerF(p);

}

/**
 * 如果模板参数是万能引用（使用了右值引用的语法），且传入的参数是左值，则T会被推导成对应的引用类型
 * 如果传入的参数是右值，则按照⬆正常的处理方式进行类型推导
 */
void ParamTypeIsAUniversalRefer() {
    std::cout << "<<<< ParamType is a universal reference" << std::endl;
    int x = 27;
    const int cx = x;
    const int& rx = x;

    UniversalF(x);
    UniversalF(cx);
    UniversalF(rx);
    UniversalF(27);
}



/**
 * 如果模版参数仅仅是一个值类型，即相当于值传递，则实参的引用属性和const、volatile属性会被忽略
 */
void ParamTypeIsTypeOnly() {
    std::cout << "<<<< ParamType is a type only" << std::endl;
    int x = 27;
    const int cx = x;
    const int &rx = x;
    const char* const p = "Fun with pointers";

    TypeOnlyF(x);
    TypeOnlyF(cx);
    TypeOnlyF(&rx);
    TypeOnlyF(p);
}

/**
 * 如果实参是数组类型，按值传递的模板函数会将数组退化为const char*，而如果传入参数为引用的模板函数，参数类型为数组的引用！
 * 其中数组的长度会被保留，因此可以实现一个模板函数获得数组的长度
 */
void ArrayArgument() {
    std::cout << "<<<< Argument is an array" << std::endl;
    const char name[] = "J. P. Briggs";
    TypeOnlyF(name);
    NoConstF(name);
}


/**
 * 获取数组长度的模板函数
 */
template <typename T, std::size_t N>
constexpr std::size_t ArraySize(T (&)[N]) noexcept {
    return N;
}

/**
 * 如果实参是函数类型，按值传递的模板函数会退化为函数指针，引用模板参数则
 */
void someFunc(int, double) {}
void FuncArgument() {
    std::cout << "<<<< Argument is a function" << std::endl;
    TypeOnlyF(someFunc);
    NoConstF(someFunc);
}


int main() {
    ParamTypeIsAPointerOrRefer();
    ParamTypeIsAUniversalRefer();
    ParamTypeIsTypeOnly();
    ArrayArgument();
    FuncArgument();
}
