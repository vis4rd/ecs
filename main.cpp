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
    struct C4{};
    struct C5{};
    struct C6{};
    struct C7{};
    struct C8{};
    struct C9{};
    struct C10{};
    struct C11{};
    struct C12{};
    struct C13{};
    struct C14{};
    struct C15{};
    struct C16{};
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
    struct D10{};
    struct D11{};
    struct D12{};
    struct D13{};
    struct D14{};
    struct D15{};
    struct D16{};
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
    struct E10{};
    struct E11{};
    struct E12{};
    struct E13{};
    struct E14{};
    struct E15{};
    struct E16{};
    struct F0{};
    struct F1{};
    struct F2{};
    struct F3{};
    struct F4{};
    struct F5{};
    struct F6{};
    struct F7{};
    struct F8{};
    struct F9{};
    struct F10{};
    struct F11{};
    struct F12{};
    struct F13{};
    struct F14{};
    struct F15{};
    struct F16{};
    using Types = ecs::meta::TypeList<C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16,
                                    D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15, D16,
                                    E1, E2, E3, E4, E5, E6, E7, E8, E9, E10, E11, E12, E13, E14, E15, E16,
                                    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16>;

    std::cout << "SIZE = " << ecs::meta::TypeListSize<Types> << std::endl;
    std::cout << "INDEX = " << ecs::meta::IndexOf<C1, Types> << std::endl;
    /*std::cout << "TYPE = " << ecs::util::type_name_to_string<Types::TypeAt<3>>() << std::endl;*/

    return 0;
}
