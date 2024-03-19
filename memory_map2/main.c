#include <dlfcn.h>

#include "foo.h"
#include "bar1.h"
#include "bar2.h"

void my_func()
{
	void *handle;
	void (*bar2)(void);

	foo();
	bar1();

	handle = dlopen("libbar2.so",  RTLD_NOW | RTLD_LOCAL);
	*(void **)(&bar2) = dlsym(handle,"bar2");
	if (bar2) 
	{
		bar2();
	}
	dlclose(handle);
}

int main()
{
	my_func();
	
	return 0;
}

