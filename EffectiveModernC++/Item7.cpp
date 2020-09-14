/**
 * @file Item7.cpp
 * @brief 在创建对象的时候注意区分{}和()
 * @date 2020/9/14
 */

#include <vector>

class Widget {
    // int c(2); // 小括号不能用来初始非静态成员
public:
    Widget();
    Widget(long i, double j);
    Widget(std::initializer_list<long double> t);
};

/*
 * - 小括号不能初始化类中的非静态成员
 * - 大括号可以阻止隐式的窄化类型检查，也免疫解析语法
 * - 只要有任何可能，大括号都会将其与带有std::initializer_list的构造函数匹配
 */
int main() {
    /*
     * C++11 引入了统一初始化的概念，使用大括号就可以进行统一初始化
     */
    std::vector<int> v{1, 2, 3}; // 使用{}初始化vector的内容
    /* 大括号可以用于初始化非静态成员，但是不能使用小括号 */
    int c{2};
    /* 大括号会执行窄化型别类型检查，如果进行窄化会编译不通过 */
    double x = 0, y = 1.0, z = 2;
    // int d{x + y + z};

    /* 而小括号和等号就不会检查*/
    int d(x + y + z);

    /* 而且大括号免疫解析语法 */
    Widget w{};
    // Widget x(); // 这种会被解析成定义了一个返回Widget的函数，名为x

    /* 大括号的缺点：
     * 如果类的构造函数支持std::initializer_list参数，则大括号会尽可能的通过该构造函数构造实例，即使需要执行窄化型别转换也会使用
     * 该构造函数。只有找不到任何办法将大括号内的实参转换为std::initializer_list模板中的类型时，才会执行其他构造函数
     */

    /*
     * 如果使用一个空的{}来构造对象，其表示的是空的实参呢还是一个空的std::initializer_list呢？语言规定是空的实参，如果想传入空的std::initializer_list，
     * 需要使用{{}}
     */
    Widget empty{};
    Widget emptyList{{}};

    Widget w1({});

    /*
     * 作为类的设计者，一般要将构造函数设计成使用大括号还是小括号都不会影响调用的函数。
     */
    return 0;
}

