#include "assembler_pass1_header.h"
#include<stdio.h>
int main(){
	FILE *f=fopen("intermediate_file.txt","w");
	fclose(f);
	assembler_pass1();
	return 0;
}

