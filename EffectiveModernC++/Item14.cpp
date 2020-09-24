/**
 * @file Item14.cpp
 * @brief 只要函数不会发送异常，就为其加上noexcept声明
 * @date 2020/9/21
 */

#include <cstddef>

/*
 * 对函数声明noexcept会让编译器生成更好的目标代码
 * 在带有noexcept声明的函数中，优化器不需要在异常传出函数的前提下，将执行期栈保持在可开解状态
 */

class Widget {

    int widget = 0;

    /**
     * 移动构造函数/赋值运算符加上noexcept能够在vector等容器的转移过程中被调用，
     * 如果移动构造函数没有声明noexcept，则vector默认会使用复制运算符而不是移动运算符，因为
     * vector需要进行强安全保证，保证转移过程中如果出现错误不会影响原来的数据
     */
    Widget(Widget&& w) noexcept {
        widget = w.widget;
    }

};

/*
 * noexcept嵌套
 * swap数组函数是否是noexcept，取决于交换数组中的单个元素是否是noexcept
 */
template <class T, size_t N>
void swap(T (&a)[N], T(&b)[N]) noexcept(noexcept(swap(*a, *b)));

/*
 * 一般情况下，函数都是异常中立的，也就是说函数本身并不会抛出异常，但是会调用有可能抛出异常的函数，异常中立函数永远不会具有noexcept标志
 * 特殊情况：
 * - 在C++11中，所有析构函数隐式具有noexcept标志，除非类中有数据成员的析构函数显式声明为noexcept(false)
 * - 对于具有宽松契约的函数来说（就是与程序运行状态无关，也对传入的实参没有限制），如果知道它不会发射异常，可以标为noexcept
 * - 对于狭隘契约的函数来说，由于需要检查实参，并且有可能具有一定的前置条件，因此不宜标记为noexcept
 *
 * 编译器允许函数声明noexcept的同时调用可能会抛出异常的函数，有可能是因为调用的函数在文档中已经声明不会发射异常（但是没有声明noexcept），
 * 也有可能是来自C语言的库等等情况。
 */
int main() {
    return 0;
}