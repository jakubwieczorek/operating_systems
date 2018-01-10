#include<stdlib.h>
#include<time.h>
#include<stdio.h>

int main(void)
{
	srand(time(NULL));

	printf("%d\n", rand() % 7 + 1);
}
