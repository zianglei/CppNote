/**
 * @file Item13.cpp
 * @brief Prefer const_iterators to iterators
 * @date 2020/9/20
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
 * 当不需要修改迭代器指向的内容时，尽可能使用const_iterator代替iterator
 * 在C++98中，获取容器的const_iterator非常困难，并且使用iterator指示位置的函数（例如insert等）只接受iterator类型，不接受
 * const_iterator类型，导致编程人员仅在必要的场合才使用const_iterator
 *
 * 在C++11中，容器引入了cbegin, cend等成员函数，从而可以很方便的使用const_iterator
 */


/**
 * C++14的写法
 */
template <typename T, typename V>
void findAndInsert(T& container,
                   const V& targetVal,
                   const V& insertVal) {
    using std::cbegin;
    using std::cend;
    auto it = std::find(cbegin(container), cend(container), targetVal);
    container.insert(it, insertVal);
}

/**
 * C++11中const_iterator的一个缺点是：在写一些模板函数或者库函数的时候，对于没有在成员函数中提供cbegin等函数的容器来说（例如数组），需要调用非
 * 成员函数版本的cbegin, cend等等函数，但是这些函数只在C++14中提供了，在C++11中仅提供了非成员函数版本的begin,end函数。那如果需要实现
 * 类似cbegin的效果，如下所示：
 *
 * 当container是数组的时候，std::begin存在特化，返回指向数组第一个元素的指针，由于传入的container已经被推导为const，
 * 因此返回的就是const pointer
 */
template <class C>
auto cbegin(const C& container) -> decltype(std::begin(container)) {
    return std::begin(container);
}

int main() {
    std::vector<int> vec;
    for(auto it = vec.cbegin(); it != vec.cend(); ++it) {
        // do something
    }

    int arr[] = {1, 2, 3};
    cbegin(arr);
}
