#include <stdio.h>


int *Foo();
int *Bla();

typedef int *(* func)(void);
int p = 5;

int main()
{
	func func1 = Foo;
	printf("*Foo() %d\n", *Foo());
	printf("Foo() %p\n", (void *)Foo());
	func1 = Bla;
	printf("Bla: %p\n", func1);
	func1 = &Bla;
	printf("&Bla: %p\n", func1);
	*Foo() = 7;
	printf("%d\n",p);

	return 0;
}

int *Foo()
{
	return (&p);
}

int *Bla()
{
	return (&p);
}