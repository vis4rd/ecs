# Entity Component System
This repository is an attempt to implement a basic/naive version of Entity Component System.<br>
<br>
## What is it?
It's a structural principle which favors data separation from application logic over inheritance and object mutual relations. It's very often combined with data-oriented-design approach as objects don't hold their data, but only reference to it.<br>
<br>
Read more about ECS [here](https://en.wikipedia.org/wiki/Entity_component_system "Entity Component System - Wikipedia").<br>
<br>
## Why?
I've decided to challenge myself with a project which would include meta-programming in C++ and (potentially) basics of multithreading. Another reason would be to learn features of C++17 standard.<br>
<br>
## Compiling
Because this project does not require any dependencies, it is fairly simple to build. Note that C++17 capable compiler is required (tested with GNU 10.3.0-1ubuntu1). The whole process is handled by CMake.<br>
```bash
$ git clone https://github.com/vis4rd/ecs.git
$ cd ecs
$ mkdir cmake && cd cmake
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
```
Now run the program with:
```bash
$ ../bin/ECS 1
```
<br>
The program consists of the ECS implementation and performance test. Generally, it is recommended to compile it with `Release` flag, since compiler does some aggresive optimizations. Besides that, the test is also (sort of) checking cache friendliness. To perform a complete performance analysis, run the program without any parameters.<br>
<br>
## License
To be added.
