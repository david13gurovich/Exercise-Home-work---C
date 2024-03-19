#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "wd.h"

/*int PrintNum();

int main(int argc, char const *argv[])
{
	StartWd(argc, argv);
	printf("%d\n", getppid());

	PrintNum();
	
	return (0);
}

int PrintNum()
{
	char *a = malloc(sizeof(char) * 100);
	while (1)
	{
		scanf("%s", a);
		if (strcmp(a, "exit") == 0)
		{
			return(1);
		}
	}

	return (0);
}*/

int factorial(int n) 
{
   if(n == 0) 
   {
      return 1;
   } 
   
   return n * factorial(n-1);
}

int fibbonacci(int n) {
   if(n == 0)
   {
      return 0;
   } 
   
   if(n == 1) 
   {
      return 1;
   } 
   else 
   {
      return (fibbonacci(n-1) + fibbonacci(n-2));
   }
}

int main(int argc, char const *argv[])
{
   void *param = NULL;
   int n = 42;
   int i;

   StartWd(argc, argv, &param);
   printf("%p\n", param);
	
   for(i = 0; i < n;i++) 
   {
   		printf("%d\n", i);
   		printf("%d ",fibbonacci(i));            
   }
   
   StopWd(param);

   puts("FINISH! \n");

   return 0;
}
