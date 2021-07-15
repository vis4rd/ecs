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
    using types = ecs::meta::TypeList<C0, C1, C2, std::string, std::vector<int>, C0, C1, int>;
    ecs::meta::TupleFromTypeList<types> type_tuple{};

    ecs::meta::print_TypeList<types>();  
    return 0;
}
