# Overview

This project uses CMake, along with the following libraries:

* Spdlog
* Glbinding
* Eigen
* Assimp
* Nlohmann JSON

# Build Instructions

Building this project requires an internet connection - it will download and build much of its dependencies. 

## Linux

Go to the project directory, and do:
`mkdir build`
`cd build`
`cmake ..`
'make -j8`
`./project1`

## Windows

The c++/CMake tooling for visual studio should be installed. 
You can right click in the project folder, and open the project with Visual Studio 2019. Then, you can select the project1 target, and run as usual.

Alternatively, the CMake GUI (or cli) can be used to generate a traditional visual studio project. 

In order to avoid showing a console during execution, a traditional visual studio project has to be generated. 