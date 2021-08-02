#include <iostream>
#include "include/Entity.h"
#include "include/Component.h"
#include "include/Manager.h"
#include "include/ComponentBuffer.h"

void test_fun1(int &arg1)
{
    arg1 = 5;
}

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

void test_fun2(C9 &arg1)
{
    arg1.data = 1337;
}

int main()
{
    using CP = ecs::meta::ComponentPool<int, float, char, C3, C4, C5, C6, C7, C8, C9,
        D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,
        E0, E1, E2, E3, E4, E5, E6, E7, E8, E9>;

    ecs::Manager<CP> manager;
    for(ecs::uint32 i = 0; i < ecs::uint32{1000}; i++)
    {
        manager.addEntity<30>(ecs::uint64{0xFFFFFFFFFFFFFFFF}, ecs::uint64{1});
    }

    auto &comp1 = manager.getComponent<9>(200);
    comp1.data = 2;
    manager.checkComponent<9>(200);

    // THE GRAND TEST OF SYSTEM SUPPORT
    std::function<void(int &)> fun = test_fun1;
    std::function<void(C9 &)> fun2 = test_fun2;
    manager.applySystem<int>(fun);
    manager.applySystem<C9>(fun2);
    std::cout << manager.getComponent<int>(1) << std::endl;
    std::cout << manager.getComponent<C9>(789).data << std::endl;
    std::function<void(char &)> fun3 = [&](char &c){ c = 'c'; };
    manager.applySystem<char>(fun3);
    //

    for(ecs::uint32 i = ecs::uint32{0}; i < ecs::uint32{1000}; i++)
    {
        manager.deleteEntity(ecs::uint64(i));
    }

    std::cout << "SIZE = " << ecs::meta::TypeListSize<CP> << std::endl;
    std::cout << "INDEX = " << ecs::meta::IndexOf<C4, CP> << std::endl;
    // ecs::meta::metautil::Print<Types>();
    
    return 0;
}
