/**
 * @file Item8.cpp
 * @brief 优先选用nullptr，而非0或NULL
 * @date 2020/9/15
 */

#include <mutex>
#include <iostream>

void f(int a) { std::cout << "f(int) called" << std::endl; }
void f(void* a) { std::cout << "f(void*) called" << std::endl; }

using MuxGuard = std::lock_guard<std::mutex>;

struct Widget {
    int a;
};

int f1(std::shared_ptr<Widget> spw);
double f2(std::shared_ptr<Widget> spw);
bool f3(Widget* pw);

template <typename FuncType, typename MuxType, typename PtrType>
auto lockAndCall(FuncType func, MuxType& mutex, PtrType ptr) -> decltype(func(ptr)) {
    MuxGuard g(mutex);
    return func(ptr);
}

/*
 * - 优先选用nullptr表示空指针
 * - 不要实现整形和指针型别之间的类别转换
 */
int main() {
    /* 在标准中，NULL可以定义为非int型（比如long），并且0和NULL都不具备指针类型 */
    /* 如果向指针形参传入0/NULL，则编译不通过 */

    f(0);
//    f(NULL);
    f(nullptr);

    /* nullptr可以很好的提示函数返回的类型为指针类型，并且在模板中，需要指针的场合只能使用nullptr*/
    std::mutex f1m, f2m, f3m;
    // auto result1 = lockAndCall(f1, f1m, 0); // 报错，无法找到将0转换为指针的方法
    // auto result2 = lockAndCall(f2, f2m, NULL); // 同样的报错
    auto result3 = lockAndCall(f3, f3m, nullptr);
}

