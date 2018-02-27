#include "assembler_pass1_header.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int STARTADR=0;
int errorFlag=0;

instruction readInstruction(char* line){
	const char delim[]=" \t\n";
	instruction readIns;
	char *token;
	int numOfTokensRead=0;	/*This can take a maximum value of 3*/
	token=strtok(line,delim);
	while(token!=NULL){
		if(numOfTokensRead==0){
			readIns.LABEL=strdup(token);	
			/*
			** create a separate heap space of size of *token and copy token data into it. 
			** After that, assign *LABEL to that heap space. That is what strdup does.
			*/
		}
		else if(numOfTokensRead==1){
			readIns.OPCODE=strdup(token);
		}
		else{
			readIns.OPERAND=strdup(token);
		}
		token=strtok(NULL,delim);
		numOfTokensRead++;
	}
	/*
	** The logic here is simple. If 3 tokens are read, then LABEL, OPCODE and OPERAND are assigned correct values.
	** If it is less than 3, then either LABEL or OPERAND or both were missing.
	** Hence, in other cases, assign correct values to LABEL, OPCODE and OPERAND as shown below.
	*/
	if(numOfTokensRead==3){
		return readIns;
	}
	else if(numOfTokensRead==2){
		readIns.OPERAND=readIns.OPCODE;
		readIns.OPCODE=readIns.LABEL;
		readIns.LABEL=NULL;
	}
	else{
		readIns.OPCODE=readIns.LABEL;
		readIns.LABEL=NULL;
		readIns.OPERAND=NULL;
	}
	free(token);
	return readIns;
}

/* 
** This function reads the source file and returns all the instructions in proper format as LABEL, OPCODE and OPERAND 
** This function is where the first pass of the source program occurs. "pass 1" of assembler.
*/
instruction* getAllInstructions(char sourceFilename[]){
	char *line=(char*)malloc(300); /*allocate 300 bytes to *line. This means, each line in the source file may contain max of 300 characters.*/
	instruction *ins=(instruction*)malloc(MAX_INSTRUCTIONS*sizeof(instruction));	
	/* 
	** Let's read a maximum of 100 lines of SIC assembly code. 
	** The above line of code is equivalent to an array of 'instruction' of size 100 i.e., instruction[100].
	** But, using instruction[100] instead of malloc will lead to segmentation fault because,
	   you will be returning 'ins' array i.e., ins[100], (as a pointer) at the end of this function but that ins array will be removed from the stack when          
	   getAllInstructions() ends.
	** So, when the ins pointer variable in main function receives the address, it will be an invalid address as it was removed from the stack earlier.
	** Hence, when you access ins in main function, you will end up with segmentation fault. 
	   But, malloc allocates space for ins in the heap and it will not be removed until it is "freed".
	*/
	int index=0;
	FILE *sourceFile=fopen(sourceFilename,"r");
	while(fgets(line,301,sourceFile)){
		ins[index]=readInstruction(line);
		index++;
	}
	ins[index].LABEL=NULL;
	ins[index].OPCODE=NULL;
	ins[index].OPERAND=NULL;
	numberOfInstructions=index-1;
	free(line);
	return ins;
}

/*This function writes intermediate instruction to the file*/
void writeIntermediateInstructions(int lineNumber,int addr,instruction ins){
	intermediateInstructions[lineNumber].address=addr;
	intermediateInstructions[lineNumber].ins=ins;
	intermediateInstructions[lineNumber].objCode=NULL;
	FILE *f_inter=fopen("intermediate_file.txt","a+");
	fprintf(f_inter,"%X\t%s\t%s\t%s\n",intermediateInstructions[lineNumber].address,ins.LABEL,ins.OPCODE,ins.OPERAND);
	fclose(f_inter);
}

/*This function searches for duplicate labels. If duplicates found, then returns -1, else returns the index of the empty field to insert next (LABEL,LOCCTR)*/
int searchSYMTAB(char* label,int symtabIndex){
	int i;
	for(i=0;i<symtabIndex;i++){
		if(strcmp(SYMTAB[i].LABEL,label)==0){
			/*if found, then set errorFlag to 1*/			
			errorFlag=1;
			return errorFlag;
		}
	}
	return 0;
}

void writeSYMTAB(){
	FILE *f_symtab=fopen("symtab.txt","w");
	int SYMTABindex=0;
	printf("\n\n[[ SYMTAB ]]\n");
	printf("-----------------\n");
	printf("Label | Address (in Hex)\n");
	printf("-----------------\n");
	while(SYMTAB[SYMTABindex].LABEL!=NULL){
		printf("%s\t|\t%X\n",SYMTAB[SYMTABindex].LABEL,SYMTAB[SYMTABindex].address);
		fprintf(f_symtab,"%s\t|\t%X\n",SYMTAB[SYMTABindex].LABEL,SYMTAB[SYMTABindex].address);
		SYMTABindex++;
	}
	fclose(f_symtab);
	printf("\nSYMTAB is written to symtab.txt\n");
}

/*This function reads the Opcodes and their mnemonics from the text file which is given by the user.
  It then creates a datastructure called OPTAB which is a global variable which can be used anywhere throughout this file.
  It also initializes noOfOpcodes variable which holds the number of opcodes that are given by the user.
*/
void readOPTAB(){
	printf("\n\n[[ OPTAB ]]\n");
	printf("-----------------\n");
	printf("Mnemonic | Opcode\n");
	printf("-----------------\n");
	const char delim[]=" \t\n";
	int numOfTokensRead;
	FILE *f_optab=fopen("OPCODE.txt","r");
	char* line=(char*)malloc(16);	//read upto 16 characters
	char* token;
	OPTAB=(opcode_table*)malloc(MAX_OPCODES*sizeof(opcode_table));
	numberOfOpcodes=0;
	while(fgets(line,16,f_optab)){
		numOfTokensRead=0;
		token=strtok(line,delim);
		
		while(token!=NULL){
			numOfTokensRead++;
			if(numOfTokensRead==1){
				OPTAB[numberOfOpcodes].mnemonic=strdup(token);
				
			}
			else if(numOfTokensRead==2){
				OPTAB[numberOfOpcodes].OPCODE=strdup(token);
			}
			token=strtok(NULL,delim);
		}
		printf("  %s        %s\n",OPTAB[numberOfOpcodes].mnemonic,OPTAB[numberOfOpcodes].OPCODE);
		numberOfOpcodes++;
		
	}
}

int searchOPTAB(char* opcode){
	int i;
	for(i=0;i<numberOfOpcodes;i++){
		if(strcmp(OPTAB[i].mnemonic,opcode)==0){
			return 1;
		}
	}
	return 0;
}

int lengthOfConstantInBytes(char* constant){
	if(constant[0]=='X' || constant[0]=='x'){
		return ((strlen(constant)-3)*4)/8;
	}else if(constant[0]=='C' || constant[0]=='c'){
		return (strlen(constant)-3);
	}
	return 0;
}

void assembler_pass1(){
	/*read the source file and get the instructions*/
	instruction* instructions=getAllInstructions("source_assembly.txt"); 
	/*initialize intermediate instructions global variable.*/
	intermediateInstructions=(intermediate_instruction*)malloc(numberOfInstructions*sizeof(intermediate_instruction));
	/*initialize SYMTAB global variable*/
	SYMTAB=(symbol_table*)malloc(numberOfInstructions*sizeof(symbol_table));
	/*keep track of the line number. Let the first line be zero*/
	int lineNumber=0; 
	/*SYMTABindex will hold the value of index of the empty field where the new (LABEL,LOCCTR)  will be inserted*/
	int SYMTABindex=0;
	/*Read the Opcode table*/
	readOPTAB();
	/*
		#PASS 1 algorithm starts from here
	*/
	/* check if the opcode of first line of the file is 'START' */
	if(strcmp(instructions[lineNumber].OPCODE,"START")==0){
		/* save #[operand] as starting address. The address given in the source file is by default taken as Hex and is converted to decimal. */ 
		sscanf(instructions[lineNumber].OPERAND,"%X",&STARTADR);	
		printf("\n\nSTARTADR=%d <-(Decimal) %X <-(Hex)\n",STARTADR,STARTADR);
		/*initialize LOCCTR to starting address*/
		LOCCTR=STARTADR;
		/*write line to intermediate file*/	
		writeIntermediateInstructions(lineNumber,LOCCTR,instructions[lineNumber]);
		/*read next input line*/
		lineNumber++;
		
	}
	else{
		/*if OPCODE != 'START', assign the default value of STARTADR, i.e., zero*/
		LOCCTR=STARTADR; 
		/*read next input line*/
		lineNumber++;
	}
	/*This variable will hold the address of the instruction currently being read
	  We already have LOCCTR, but why do we require another variable to hold address?
	  This is because,LOCCTR holds the address of the next instruction and not the current instruction. 
	  LOCCTR is like the program counter.
	*/
	int currentInstructionAddress;
	/*while OPCODE !='END'*/
	while(strcmp(instructions[lineNumber].OPCODE,"END")!=0){
		currentInstructionAddress=LOCCTR;
		/*if there a symbol in the LABEL field*/
		if(instructions[lineNumber].LABEL!=NULL){
			/*Search SYMTAB for LABEL. This function returns -1 if the LABEL is found, i.e. if there is an error. Else, it returns the index of the empty field in SYMTAB*/
			int labelFound=searchSYMTAB(instructions[lineNumber].LABEL,SYMTABindex);
			/*If error flag is set, then display error for duplicate labels*/
			if(labelFound){
				/*lineNumber started from 0. Hence lineNumber+1 is the appropriate lineNumber*/
				printf("\nERROR: Duplicate symbol '%s' found at line number %d\n",instructions[lineNumber].LABEL,lineNumber+1);
				exit(1);	
			}
			else{
				/*insert (LABEL,LOCCTR) into SYMTAB*/
				SYMTAB[SYMTABindex].LABEL=strdup(instructions[lineNumber].LABEL);
				SYMTAB[SYMTABindex].address=currentInstructionAddress;
				SYMTABindex++;
			}
		}
		int found=searchOPTAB(instructions[lineNumber].OPCODE);
		if(found){
			/*add 3 {instruction length} to LOCCTR*/
			LOCCTR=LOCCTR+3;
		}else if(strcmp(instructions[lineNumber].OPCODE,"WORD")==0){
			/*add 3 to LOCCTR*/
			LOCCTR=LOCCTR+3;
		}else if(strcmp(instructions[lineNumber].OPCODE,"RESW")==0){
			/*add 3 * #[OPERAND] to LOCCTR*/
			LOCCTR=LOCCTR+(3*atoi(instructions[lineNumber].OPERAND));
		}else if(strcmp(instructions[lineNumber].OPCODE,"RESB")==0){
			/*add #[OPERAND] to LOCCTR*/
			LOCCTR=LOCCTR+atoi(instructions[lineNumber].OPERAND);
		}else if(strcmp(instructions[lineNumber].OPCODE,"BYTE")==0){
			/*find the length of constant in Bytes, add length to LOCCTR*/
			LOCCTR=LOCCTR+lengthOfConstantInBytes(instructions[lineNumber].OPERAND);
		}else{
			printf("\nERROR: Invalid Opcode \"%s\" found at line number %d\n",instructions[lineNumber].OPCODE,lineNumber+1);
			exit(1);
		}
		/*write line to intermediate file*/	
		writeIntermediateInstructions(lineNumber,currentInstructionAddress,instructions[lineNumber]);
		/*read next input line*/
		lineNumber++;
	}
	writeSYMTAB();
	printf("Intermediate file is written to intermediate_file.txt\n");
}


