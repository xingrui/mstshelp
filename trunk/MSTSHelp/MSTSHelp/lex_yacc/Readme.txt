�ļ��б�

MinGW		  GNU��C/C++�������windows�汾�� ���SVN��û�У���Ҫ�Լ�����
bison.exe         GNU��yacc����
bison.hairy       GNU��yacc����������Ҫ���ļ�
bison.simple      GNU��yacc����������Ҫ���ļ�
flex.exe          GNU��lex����
ini.bat           ���lex��yacc�����Ļ�����������
lexyacc.bat       ���lex��yacc��������������
Readme.txt        ��˵���ļ�

ʹ�÷�����

1. ���˫��lexyacc.bat�ļ�����
2. �ڵ�����DOS������������CD�л������lex��yaccԴ�ļ����ڵ�Ŀ¼(compiler)
3. ��lexԴ�ļ�ִ��flex calc.l
4. ��yaccԴ�ļ�ִ��bison -d calc.y
5. g++ lex.yy.c c.tab.c -o c

