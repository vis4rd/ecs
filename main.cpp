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

    //
    std::cout << std::endl;
    //

    // TBD : simplify the examples and move them to ComponentBuffer.h as documentation ////////////

    using Test2 = ecs::meta::ComponentPool<int, float, char, long long, double>;
    ecs::ComponentBuffer<Test2> CB2;
    // entity 1 = int + float + char
    // entity 2 = int + long long + double
    auto &ref = CB2.addComponent<int>(ecs::uint64{1});
    auto &ref2 = CB2.addComponent<float>(ecs::uint64{1});
    auto &ref3 = CB2.addComponent<char>(ecs::uint64{1});
    ref = 5;
    ref2 = 15.f;
    ref3 = 'c';
    CB2.addComponent<long long>(ecs::uint64{2});
    CB2.addComponent<double>(ecs::uint64{2});
    auto &hak = CB2.addComponent<int>(ecs::uint64{2});
    hak = 2;
    auto tuple = CB2.getComponentsMatching<int>(1);
    auto tuple2 = CB2.getComponentsMatching<int>(2);
    std::cout << std::get<0>(tuple) << std::endl;
    std::cout << std::get<0>(tuple2) << std::endl;

    auto tuple3 = CB2.getComponentsMatching<>(1);
    // std::cout << std::get<0>(tuple3) << std::endl;  // compilation error - tuple3 contains no types as none were provided for search

    auto tuple4 = CB2.getComponentsMatching<long long>(1);
    // std::cout << std::get<0>(tuple4) << std::endl;  // exception - there's no such component with given id

    auto tuple5 = CB2.getComponentsMatching<float>(3);  // exception - there's no such component with given id

    return 0;
}
