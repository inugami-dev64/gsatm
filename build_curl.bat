@echo off
Rem *******************************
Rem ********* Autostart ***********
Rem *******************************
Rem %1 is either visual studio version or clean keyword

if NOT "%1"=="clean" (
    cd submodules\curl\winbuild > nul
    call ..\buildconf.bat > nul
    set RTLIBCFG=static > nul
    echo Building libcurl...
    nmake /f Makefile.vc mode=static vc=%1 debug=no > nul

    Rem Check if build directory exists 
    if not exist ..\..\..\build\ (mkdir ..\..\..\build)
    if not exist ..\..\..\build\curl (mkdir ..\..\..\build\curl)
    move /y ..\builds\libcurl-vc%1-x64-release-static-ipv6-sspi-schannel\lib\libcurl_a.lib ..\..\..\build\curl\libcurl_a.lib
    xcopy /e /y /i ..\builds\libcurl-vc%1-x64-release-static-ipv6-sspi-schannel\include ..\..\..\build\curl\include\*
    cd ..\..\..\
) else (
    echo Cleaning all build configs...
    rmdir /s /q build > nul
    rmdir /s /q submodules\curl\builds > nul
)

echo Done
