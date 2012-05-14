@set INSTALLDIR=%CD%
@PATH=%INSTALLDIR%;%INSTALLDIR%\MinGW\bin
@set BISON_SIMPLE=%INSTALLDIR%\bison.simple
@set BISON_HAIRY=%INSTALLDIR%\bison.hairy
cd compiler
flex c.l
bison -v -d c.y
g++ lex.yy.c c.tab.c -o c