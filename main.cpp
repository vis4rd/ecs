#include <iostream>
#include "include/Component.h"
#include "include/Manager.h"
#include "include/ComponentBuffer.h"

struct C0{};
struct C1{};
struct C2{};
struct C3{};
struct C4{};
struct C5{};
struct C6{};
struct C7{};
struct C8{};
struct C9{int data;};
struct D0{};
struct D1{};
struct D2{};
struct D3{};
struct D4{};
struct D5{};
struct D6{};
struct D7{};
struct D8{};
struct D9{};
struct E0{};
struct E1{};
struct E2{};
struct E3{};
struct E4{};
struct E5{};
struct E6{};
struct E7{};
struct E8{};
struct E9{};

void test_fun1(int &arg1)
{
    static int d;
    arg1 = d++;
}

void test_fun2(C9 &arg1)
{
    static int b;
    arg1.data = b++;
}

void test_fun3(char &c)
{
    c = 'd';
}

float measure_time(bool &&suppressed = false)
{
    auto clock = std::chrono::high_resolution_clock::now();

    using CP = ecs::meta::ComponentPool<int, float, char, C3, C4, C5, C6, C7, C8, C9,
        D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,
        E0, E1, E2, E3, E4, E5, E6, E7, E8, E9>;

    ecs::Manager<CP> manager;
    for(ecs::uint32 i = 0; i < ecs::uint32{1000}; i++)
    {
        manager.addEntity<30>(ecs::uint64{0xFFFFFFFFFFFFFFFF}, ecs::uint64{1});
    }

    /*auto &comp1 = manager.getComponent<9>(200);
    comp1.data = 2;*/
    manager.checkComponent<9>(200);

    // THE GRAND TEST OF SYSTEM SUPPORT
    std::function<void(int &)> fun = test_fun1;
    std::function<void(C9 &)> fun2 = test_fun2;
    std::function<void(char &)> fun3 = [&](char &c){ c = 'c'; };
    manager.applySystem<int>(fun);
    manager.applySystem<C9>(fun2);
    manager.applySystem<char>(fun3);

    manager.applySystem<int>(test_fun1);
    manager.applySystem<C9>(test_fun2);
    manager.applySystem<char>(test_fun3);
    //

    manager.deleteAllEntities();

    /*std::cout << "SIZE = " << ecs::meta::TypeListSize<CP> << std::endl;
    std::cout << "INDEX = " << ecs::meta::IndexOf<C4, CP> << std::endl;*/

    auto clock2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(clock2 - clock);
    static unsigned cycle;
    if(!suppressed)
        std::cout << "cycle (" << cycle++ << ") duration: " << duration.count()/1000.f << "ms" << std::endl;
    return duration.count()/1000.f;
}

int main(int argc, char **argv)
{
    int count = (argc > 1) ? std::__cxx11::stoi(argv[1]) : 100;
    std::vector<float> times;
    for(int i = 0u; i < count; i++)
    {
        times.emplace_back(measure_time());
    }
    times.shrink_to_fit();
    std::cout << std::endl;
    std::cout << "average time: " << std::accumulate(times.begin(), times.end(), 0.f) / times.size() << "ms" << std::endl;
    std::cout << "max: " << *std::max_element(times.begin(), times.end()) << "ms" << std::endl;
    std::cout << "min: " << *std::min_element(times.begin(), times.end()) << "ms" << std::endl;
    
    return 0;
}
