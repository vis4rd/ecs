#include <iostream>
#include "include/Entity.h"
#include "include/Component.h"
#include "include/Manager.h"
#include "include/Meta.h"

int main()
{
    ecs::Entity ent;
    ecs::Manager mgr;

    struct C0{};
    struct C1{};
    struct C2{};
    struct C3{};
    
    using Types = ecs::meta::TypeList<float, C1, C2, C3, int, char>;

    std::cout << "SIZE = " << ecs::meta::TypeListSize<Types> << std::endl;
    std::cout << "INDEX = " << ecs::meta::IndexOf<C1, Types> << std::endl;
    /*std::cout << "TYPE = " << ecs::util::type_name_to_string<Types::TypeAt<3>>() << std::endl;*/

    ecs::meta::metautil::Print<Types>();

    return 0;
}
