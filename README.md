# A* Visualizer
This program visualizes the process of finding the shortest path between two cells on a grid using the *A\* search algorithm*.
## Cloning instructions
The project uses **OpenGL** with **GLFW** and **glad** for rendering. **GLFW** is added as a submodule so the `--recurse-submodules` flag must be used when cloning:
```
git clone --recurse-submodules https://github.com/qqpilla/A-Star-Visualizer
```
Alternatively, in case you cloned the repo without specifying the `-recurse-submodules` flag you can run the following commands:
```
git submodule init
git submodule update
```
or the shorter version:
```
git submodule update --init
```
## Build instructions
[**CMake**](https://cmake.org/download/) must be installed on your system for building the application.

Create a `build` directory and `cd` into it. To generate **CMake** files use this command:
```
cmake -G <generator-name> <path-to-source>
```
where you need to specify the generator you want to use and a path to the project's root folder. For example, when running the command from the `build` directory that is a child of the project's root directory for building the program with **Visual Studio 2022** the syntax is as follows:
```
cmake -G "Visual Studio 17 2022" ..
```
The `-G <generator-name>` option may be omitted. **CMake** will select a compiler itself depending on your system. For a list of all compilers accessible on your platform you can use `cmake --help` command.

Finally, to build the project run ```cmake --build .``` from the `build` directory. You will find the executable called **program** inside the **build** directory or one of its subdirectories (depending on the generator used) 
## Controls
- Press the **Space bar** to switch between placing *Start*/*Finish* cells and *blocking*/*unblocking* cells.
- Click/hold the **Left Mouse Button** to place the *Start* cell or to *block* a cell.
- Click/hold the **Right Mouse Button** to place the *Finish* cell or *unblock* a blocked cell.
- Press the **R** key to reset the scene.
- Press the **C** key to cancel the algorithm processing.
- Press the **Escape** key to exit the application. 
