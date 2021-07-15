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
    using Types = ecs::meta::TypeList<C0, C1, C2, std::string, std::vector<int>>;

    std::cout << "SIZE = " << Types::TypeCount << std::endl;
    ecs::meta::print_TypeList<Types>();

    std::cout << "INDEX = " << Types::IndexOfType<C0>() << std::endl;
    return 0;
}
