#include <iostream>
#include "include/Entity.h"
#include "include/Component.h"
#include "include/Manager.h"
#include "include/ComponentBuffer.h"

int main()
{
    struct C0{};
    struct C1{};
    struct C2{};
    struct C3{};
    struct C4{};
    struct C5{};
    struct C6{};
    struct C7{};
    struct C8{};
    struct C9{};
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

    using CP = ecs::meta::ComponentPool<C0, C1, C2, C3, C4, C5, C6, C7, C8, C9,
        D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,
        E0, E1, E2, E3, E4, E5, E6, E7, E8, E9>;

    using CP2 = ecs::meta::ComponentPool<int, char, float, double, bool>;

    ecs::Manager<CP2> manager;
    manager.initComponentBuffer();
    for(ecs::uint32 i = 0; i < ecs::uint32{1000}; i++)
    {
        manager.addEntity(ecs::uint64{0xFFFFFFFFFFFFFFFE}, ecs::uint64{1});
    }

    manager.getEntity(555);
    manager.getComponent(4, 555);
    manager.addComponent(63, 999);
    manager.getComponent(63, 999);
    manager.deleteEntity(999);
    std::cout << manager.bufferSize() << std::endl;
    auto &com = manager.getComponent(2, 998);
    auto &vec = manager.getComponentBucket(1);
    // manager.printComponentBuffer();

    /*for(ecs::uint32 i = ecs::uint32{0}; i < ecs::uint32{1000}; i++)
    {
        manager.deleteEntity(i);
    }*/

    manager.deleteAllEntities();

    std::cout << "SIZE = " << ecs::meta::TypeListSize<CP2> << std::endl;
    std::cout << "INDEX(double) = " << ecs::meta::IndexOf<double, CP2> << std::endl;
    
    return 0;
}
