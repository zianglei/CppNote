/**
 * @file Item9.cpp
 * @brief 优先使用别名声明而非使用typedef
 * @date 2020/9/17
 */

/*
 * 别名声明的优势在于支持模板，而typedef想要支持模板必须得嵌套在struct里面
 */

#include <vector>
#include <type_traits>

template <typename T>
struct MyAllocList {
    typedef std::vector<T> type;
};

template <typename T>
using MyAllocListU = std::vector<T>;

/*
 * 更糟的情况是如果想在模板内创建一个链表，容纳的类型由模板形参指定，则前面必须加typename，
 * 因为MyAllocList<T>代表一个依赖T的型别，但是MyAllocList<T>::type可不一定也是一个型别，也有可能是一个数据成员，
 * 因此需要使用typename来确定MyAllocList<T>::type是一个型别
 *
 * 如果使用的是using别名声明，则不需要使用typename
 */
template <typename T>
class Widget {
private:
    typename MyAllocList<T>::type list;
};

/*
 * 在C++11中，type_traits提供了remove_reference<T>::type等模板来提供去掉引用等等的变量类型，但是由于内部是使用typedef声明的，
 * 需要在使用的时候加上typename关键字
 * 而在C++14中，可以使用remove_reference_t<T>来去掉typename
 */
template <typename T>
void func(T&& x) {
    typedef typename std::remove_reference<T>::type TR;
    using TRR = std::remove_reference_t<T>;

    TRR y;
}

int main() {


}

