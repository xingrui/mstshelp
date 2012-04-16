#include <iostream>
using namespace std;
class A
{
public:
	virtual void f() = 0;
	virtual void g() = 0;
};
class B : public A
{
	void f()
	{
		cout << "!!!B f!!!" << endl;
	}
	void g()
	{
		cout << "!!!B g!!!" << endl;
	}
};
class C : public B
{
	void f()
	{
		cout << "!!!B f!!!" << endl;
	}
	void g()
	{
		cout << "!!!B g!!!" << endl;
	}
};
void call_vfunc(A *a)
{
	a->f();
	a->g();
}
int main()
{
	A *a = new B;
	call_vfunc(a);
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
