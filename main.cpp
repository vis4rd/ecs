#include <iostream>
#include "include/Entity.h"

int main()
{
	std::cout << "Size of char : " << 				sizeof(ecs::int8) * 8 << " bits" << std::endl;
    std::cout << "Size of short int : " << 			sizeof(short int) * 8 << " bits" << std::endl;
    std::cout << "Size of int : " << 				sizeof(int) * 8 << " bits" << std::endl;
    std::cout << "Size of long int : " << 			sizeof(long int) * 8 << " bits" << std::endl;
    std::cout << "Size of long long int : " << 		sizeof(int64_t) * 8 << " bits" << std::endl;
    std::cout << "Size of unsigned char : " << 		sizeof(unsigned char) * 8 << " bits" << std::endl;
    std::cout << "Size of unsigned short int : " << sizeof(unsigned short int) * 8 << " bits" << std::endl;
    std::cout << "Size of unsigned int : " << 		sizeof(unsigned int) * 8 << " bits" << std::endl;
    std::cout << "Size of unsigned long : " << 		sizeof(unsigned long) * 8 << " bits" << std::endl;
    std::cout << "Size of unsigned long long : " << sizeof(uint64_t) * 8 << " bits" << std::endl;
    std::cout << "Size of float : " << 				sizeof(float) * 8 << " bits" <<std::endl;
    std::cout << "Size of double : " << 			sizeof(double) * 8 << " bits" << std::endl;
    std::cout << "Size of long double : " << 		sizeof(long double) * 8 << " bits" << std::endl;

    ecs::Entity ent;
     
    return 0;
}
