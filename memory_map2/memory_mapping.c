#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


extern char etext, edata, end; 

extern char __executable_start;
extern char __etext;

static int x =7;
static int y;

void foo();


int main(void) 
{
  char *ptr = malloc(2);
	int z;
	char shell_command[100];



  printf ("Address range for:\n");
  printf ("\nCode Segment:\n%10p - %10p \n" ,&__executable_start, &__etext - 1);
  printf("Example for address in code segment: %10p: \n", foo);

  printf ("\nData Segment:\n%10p - %10p \n" ,&etext, &edata - 1);
  printf("Example for address in Data segment:  %10p: \n",&x );

  printf ("\nBSS Segment:\n%10p - %10p \n" ,&edata, &end);
  printf("Example for address in BSS segment:  %10p: \n",&y );
    
  printf("\nHeap Segment: \n");
  sprintf(shell_command, "sudo cat /proc/%d/maps | grep \"heap\" | awk '{print $1}'" , getpid());
  system(shell_command);
  printf("Example for address in Heap segment:  %10p: \n",ptr );

  printf("\nStack Segment: \n");
  sprintf(shell_command, "sudo cat /proc/%d/maps | grep \"stack\" | awk '{print $1}'" , getpid());
  system(shell_command);
  printf("Example for address in Stack segment:  %10p\n",&z );

  return (EXIT_SUCCESS);
}
void foo()
{

}
void a_foo()
{

}

