/*
C++ fibonacci Recursion function
*/

#include <stdio.h>

int Fibonacci(int n)
{ 
	if (n==0)
		return 0;
	if (n==1)
		return 1;
	return( Fibonacci(n-2) + Fibonacci(n-1) );
}

void main (void)
{
	unsigned int num;
	printf("Please provide me a number: ");
	scanf("%d",&num);
	printf("Fibonacci Sequence for your number: %s",Fibonacci(num));
}