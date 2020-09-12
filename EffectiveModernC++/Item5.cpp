/**
 * @file Item5.cpp
 * @brief 优先选用auto，而非显式型别声明
 * @date 2020/9/12
 */

#include <memory>
#include <functional>
#include <string>
#include <unordered_map>

/*
 * 使用auto的好处
 * - 避免未初始化
 * - 避免兼容性和效率问题的类型不匹配现象
 * - 提高编码效率
 */
int main() {
    // auto x; // 报错，会检测没有初始化的变量
    // auto可以指定只有编译器知道的类型
    auto derefUPLess = [](const std::unique_ptr<int>& p1, const std::unique_ptr<int>& p2) {
        return *p1 < *p2;
    };

    // std::function可以存储一切符合类型的可调用之物
    std::function<bool(const std::unique_ptr<int>& p1, const std::unique_ptr<int>& p2)>
            realTypeV = derefUPLess;
    // 但是auto和std::function的区别为auto声明的闭包和实际闭包是同一类型，占用同一空间，而std::function
    // 存储的只是std::function的一个实例，占用固定的内存。如果存储不了闭包，那么每次std::function的构造函数
    // 都会在堆内存另外分配空间，效率比auto低

    // 另外，显示声明变量类型还有可能出现性能问题
    std::unordered_map<std::string, int> m;
    // unordered_map的key实际上是const std::string类型，编译器为了转化为p对应的类型，需要先将m中的pair赋值一份，再将p绑定到
    // 复制的临时变量的引用上，因此性能会显著降低
    for (const std::pair<std::string, int>& p: m) {
    }

}