/**
 * @file Item6.cpp
 * @brief 当auto推导的型别不符合要求的时候，使用显式类型初始化
 * @date 2020/9/13
 */

#include <vector>
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
 * 对于std::vector<bool>来说，其operator[]返回的对象并不是bool本身，而是std::vector<bool>::reference代理类对象，然后代理类对象
 * 会有一个隐式转换到bool类型
 * 因为std::vector<bool>进行过特化，每个bool都由一个bit表示，而不是一个bool对象
 */
std::vector<bool> features() {
    std::vector<bool> res(6);
    return res;
}

void process(bool priority) {}

/*
 * 不能使用auto来推断"隐形"代理类型的变量型别，可以使用static_cast显示转换为对应的真正类型
 * 如果发现不了隐藏的代理对象，可以在头文件的函数返回值处寻找
 */
int main() {
   bool highPriority = features()[5];
   auto autoHighPriority = features()[5]; // 此时autoHighPriority并不是bool，而是代理类对象
   // reference对象返回的是一个指针，由于函数返回的是个临时对象，因此在语句结束后指针指向的内存就会释放掉，此时autoHighPriority就
   // 变成了一个空悬指针，如果后面再使用就会成为未定义行为。
   process(autoHighPriority);

   auto castAutoHighPriority = static_cast<bool>(features()[5]);
   std::cout << type_name<decltype(highPriority)>() << std::endl;
   std::cout << type_name<decltype(autoHighPriority)>() << std::endl;
   std::cout << type_name<decltype(castAutoHighPriority)>() << std::endl;

   return 0;
}