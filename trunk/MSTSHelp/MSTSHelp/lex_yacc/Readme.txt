文件列表：

MinGW		  GNU的C/C++编译程序（windows版本） 这个SVN上没有，需要自己下载
bison.exe         GNU的yacc程序
bison.hairy       GNU的yacc程序运行需要的文件
bison.simple      GNU的yacc程序运行需要的文件
flex.exe          GNU的lex程序
ini.bat           这个lex和yacc环境的环境变量配置
lexyacc.bat       这个lex和yacc环境的启动程序
Readme.txt        本说明文件

使用方法：

1. 鼠标双击lexyacc.bat文件运行
2. 在弹出的DOS命令行中利用CD切换到你的lex和yacc源文件所在的目录(compiler)
3. 对lex源文件执行flex calc.l
4. 对yacc源文件执行bison -d calc.y
5. g++ lex.yy.c c.tab.c -o c

