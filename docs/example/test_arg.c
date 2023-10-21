#include <stdio.h>

int main(int argc, char *argv[]){
	
	printf("%d\n", argc);    // number of arguments
	
	if (argc >= 2)
		printf("%s\n", argv[2]);  // argv[2] is a string
		int i = atoi(argv[2]);   // convert char to int
		printf("%d\n", i);

	return 0;
}




