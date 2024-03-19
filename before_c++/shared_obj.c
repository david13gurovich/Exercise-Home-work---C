#include <stdio.h>
#include <dlfcn.h> /* dlopen, dlsym, dlclose */

#define UNUSED(a) ((void) (a))
	
void Foo()
{
	printf("FOO!\n");
}

void Bla()
{
	printf("BLA!\n");
}

int main(int argc, char const *argv[])
{
	void *handle;
	void (*foo)(void);
	
	UNUSED(argc);

	handle = dlopen(argv[1],  RTLD_LAZY);
	*(void **)(&foo) = dlsym(handle, argv[2]);
	if (foo) 
	{
		foo();
	}
	
	dlclose(handle);

	return 0;
}
