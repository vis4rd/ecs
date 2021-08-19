#include "include/Manager.h"

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

enum
{
    F_ALIVE = ecs::uint64{1} << 0
};

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
    c = 'a' + rand() % ('z' - 'a');
}

void test_fun4(ecs::Interface &itf, float &f)
{
    std::cout << "Entity [ " << itf.index() << " ]: id( " << itf.id() << " ), flags( "
        << itf.flags() << " ), components( " << itf.components() << " )"
        << std::endl;
}

int main()
{
    ecs::uint64 ent_count = 10;
    using CP = ecs::meta::ComponentPool<
        int, float, char, C3, C4, C5, C6, C7, C8, C9,
        D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,
        E0, E1, E2, E3, E4, E5, E6, E7, E8, E9>;
   auto &manager = ecs::Manager<CP>::getInstance(ent_count);

    for(ecs::uint64 i = 0; i < ecs::uint64{ent_count}; i++)
    {
        manager.addEntity<30>(ecs::uint64{0xFFFFFFFFFFFFFFFF}, F_ALIVE);
    }

    // std::cout << manager.getFlag(F_ALIVE, 50) << std::endl;
    // manager.setFlag(F_ALIVE, 50, false);
    // std::cout << manager.getFlag(F_ALIVE, 50) << std::endl;
    // auto &flags = manager.getFlagBuffer();

    // auto &comp1 = manager.getComponent<9>(200);
    // comp1.data = 2;
    manager.template checkComponent<9>(200);

    std::function<void(int &)> fun = test_fun1;
    std::function<void(C9 &)> fun2 = test_fun2;
    std::function<void(char &)> fun3 = [&](char &c){ c = 'c'; };
    manager.template applySystem<int>(fun);
    manager.template applySystem<C9>(fun2);
    manager.template applySystem<char>(fun3);

    manager.template applySystem<int>(test_fun1);
    manager.template applySystem<C9>(test_fun2);
    manager.template applySystem<char>(test_fun3);

    manager.template applySystem<float>(test_fun4);

    manager.setFlag(F_ALIVE, 7, false);
    manager.setFlag(F_ALIVE, 9, false);
    std::cout << "Filtered entities: " << manager.deleteFilteredEntities(F_ALIVE, true);

    manager.deleteAllEntities();
    return 0;
}
