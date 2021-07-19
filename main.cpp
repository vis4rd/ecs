#include <iostream>
#include "include/Entity.h"
#include "include/Component.h"
#include "include/Manager.h"
#include "include/ComponentBuffer.h"

int main()
{
    ecs::Entity ent;
    ecs::Manager mgr;

    struct C0{};
    struct C1{};
    struct C2{};
    struct C3{};
    
    using Types = ecs::meta::ComponentPool<float, C1, C2, C3, int, char>;

    std::cout << "SIZE = " << ecs::meta::TypeListSize<Types> << std::endl;
    std::cout << "INDEX = " << ecs::meta::IndexOf<C1, Types> << std::endl;
    /*std::cout << "TYPE = " << ecs::util::type_name_to_string<Types::TypeAt<3>>() << std::endl;*/
    ecs::meta::metautil::Print<Types>();

    //
    std::cout << std::endl;
    //

    struct F1
    {
        int value = 5;
    };
    using Test = ecs::meta::ComponentPool<F1>;
    ecs::ComponentBuffer<Test> CB;
    auto &f = CB.addComponent<F1>(ecs::uint64{1});
    f.value = 11;

    std::cout << CB.getComponent<F1>(1).value << std::endl;

    return 0;
}
