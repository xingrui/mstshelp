class A
{
public:
	A(const A &b)
	{
		a = b.a;
	}
	A &operator =(const A &b)
	{
		a = b.a;
	}
	A()
	{
		a = 5;
	}
	~A()
	{
		a = 0;
	}
	int a;
	int b;
	int c;
};
A f()
{
	A a;
	a.a = 1;
	a.b = 2;
	a.c = 3;
	return a;
}
int main()
{
	A a = f();
	return 0;
}