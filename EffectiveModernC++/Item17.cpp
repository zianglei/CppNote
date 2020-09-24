#include <utility>

/**
 * @file Item17.cpp
 * @brief 理解特种成员函数的生成机制
 * @date 2020/9/24
 */

class WidgetCopy {

public:
    WidgetCopy();
    WidgetCopy(WidgetCopy& rhs) {}
};

class WidgetMove {

public:
    WidgetMove() {}
    WidgetMove(WidgetMove&& rhs) {}
};

class WidgetDestructor {
public:
    WidgetDestructor() {}
    ~WidgetDestructor() {}
    int val;
};



int main() {
    WidgetCopy c1;
    WidgetCopy c2(std::move(c1)); // 不会生成移动构造函数

    WidgetMove c3;
    WidgetMove c4(c3);      // 不会生成复制构造函数

    WidgetDestructor c5;
    c5.val = 1;
    WidgetDestructor c6(c5); // 声明析构函数会自动生成复制构造函数
    WidgetDestructor c7(std::move(c5)); // 此处的移动操作会转变为复制操作
}