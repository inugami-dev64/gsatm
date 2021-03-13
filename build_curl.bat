Rem %1 is the version of visual studio and %2 is the build directory used by cmake
cd submodules\curl\winbuild
call \..\buildconf.bat
set RTLIBCFG=static
nmake /f Makefile.vc mode=static vc=%1 debug=no
mkdir ..\..\..\%2\curl
move ..\builds\libcurl-vc%1-x64-release-static-ipv6-sspi-schannel\lib\libcurl_a.lib ..\..\..\%2\curl\libcurl_a.lib
xcopy /e /i ..\builds\libcurl-vc%1-x64-release-static-ipv6-sspi-schannel\include ..\..\..\%2\curl\include\*
cd ..\..\..\%2
