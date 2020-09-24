/**
 * @file Item15.cpp
 * @brief 只要有可能是用constexpr，就使用它
 * @date 2020/9/22
 */

/*
 * constexpr和const的区别：const只表示该变量只读，但是不保证该变量是编译期常量
 * 而constexpr保证变量的值在编译器就可知，这样变量就可以被用到要求整型常量表达式的场合中，比如
 * 数组的尺寸、整型模板实参、枚举的值、对齐规格等
 */

#include <array>

constexpr auto arraySize = 10;
std::array<int, arraySize> vec;

/*
 * 如果constexpr的对象是函数，如果传入函数的实参是编译器常量，那么函数返回的值也是编译器常量；如果是运行期的变量，则函数产生的值
 * 也是运行期的变量。
 * 在C++11中，constexpr函数被限制为只能包含不多于一个的执行语句，而在C++14中就没有了这个限制
 */

constexpr
int pow(int base, int exp) noexcept {
    return (exp == 0 ? 1 : base * pow(base, exp - 1));
}

std::array<int, pow(2, 3)> arr;

/*
 * constexpr函数仅限于传入和返回字面型别，也就是在编译期可以决定的值（void不属于这种类型，其余所有内建类型都属于）
 * 但是用户的自定义类也可以属于字面型别
 */

class Point {
public:
    constexpr Point(double xVal = 0, double yVal = 0) noexcept
    : x(xVal), y(yVal) {};

    constexpr double xValue() const noexcept { return x; }
    constexpr double yValue() const noexcept { return y; }
    void setX(double newX) noexcept { x = newX; }
    void setY(double newY) noexcept { y = newY; }

private:
    double x, y;
};

int main() {

}
