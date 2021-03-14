# Goldstein Bank ATM  
## Getting started
### Additional CMake flags
For C# test program build set the values of following variables to `True`:  
`-DGSASTM_BUILD_GUI` and `-DGSATM_GUI_CODE_TEST`

### Windows
Prerequisites:  
* CMake  
* Visual Studio     

Please make sure that x64 Native Tools Command Prompt for VS is used for the whole process, otherwise the script will probably fail since no nmake can be found.  
In order to generate Visual Studio solution follow these steps:
```
git clone --recursive https://github.com/inugami-dev64/gsatm
cd gsatm
mkdir build
.\build_curl.bat [VS VERSION] build
cmake ..
```
Where [VS VERSION] specifies the version of Visual Studio used.  
Now all that is left to do is use the solution to build the project.   

### Linux
Prerequisites:  
* CMake
* Make
* curl
* gcc

In order to generate Makefile for Linux follow these steps:
```
git clone https://github.com/inugami-dev64/gsatm
cd gsatm
mkdir build
cd build
cmake ..
make
```