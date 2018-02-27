#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define MAX_INSTRUCTIONS 100	
#define MAX_OPCODES 60
/*
 * For assembler pass 1, an intermediate file is generated along with SYMTAB (symbol table)
*/

/*
#
#
#	PREPARE THE REQUIRED DATASTRUCTURES FOR ASSEMBLER PASS 1
#
#
*/
/*An instruction contains LABEL, OPCODE and OPERAND*/
struct instruction{
	char* LABEL;
	char* OPCODE;
	char* OPERAND;
};
typedef struct instruction instruction;

/*SYMTAB is a data structure which will be used to store the symbols (labels) and their addresses (in hex)*/
struct symbol_table{
	char* LABEL;
	int address;
};
typedef struct symbol_table symbol_table;

/* This structure is used to store and write (to a file) intermediate instructions containing addresses */
struct intermediate_instruction{
	int address;
	instruction ins;
	char* objCode;
};
typedef struct intermediate_instruction intermediate_instruction;

/*OPTAB is a data structure which will be used to validate the read opcodes from the source file*/
struct opcode_table{
	char* mnemonic;
	char* OPCODE;
};
typedef struct opcode_table opcode_table;

/*
#
#
#	PREPARE THE REQUIRED PARAMETERS AND VARIABLES FOR ASSEMBLER PASS 1
#
#
*/
/*location counter*/
int LOCCTR; 
 /*the starting address is by default equal to zero*/
int STARTADR=0;
/*this contains the intermediate instructions to be written into the intermediate file*/
intermediate_instruction* intermediateInstructions;	
/*this is the symbol table*/
symbol_table* SYMTAB;	
/* total number of instructions (lines) present in the given assembly source file will be stored in this variable */
int numberOfInstructions; 
/*Error flag*/
int errorFlag=0;
/*this is the opcode table*/
opcode_table* OPTAB;
/* total number of opcodes used in OPCODE.txt (this is actually used to create the malloc(opcode))*/
int numberOfOpcodes;
/*
#
#
#	DEFINE THE REQUIRED UTILITY FUNCTIONS FOR ASSEMBLER PASS 1
#
#
*/
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
int searchSYMTAB(int lineNumber,char* label){
	int i=0;
	for(i=0;i<numberOfInstructions;i++){
		if(SYMTAB[i].LABEL==NULL){
			break;
		}
		else if(strcmp(SYMTAB[i].LABEL,label)==0){
			/*if found, then set errorFlag to -1*/			
			errorFlag=-1;
			return errorFlag;
		}
	}
	return i;
}

void writeSYMTAB(){
	FILE *f_symtab=fopen("symtab.txt","w");
	int SYMTABindex=0;
	while(SYMTAB[SYMTABindex].LABEL!=NULL){
		fprintf(f_symtab,"%s\t|\t%X\n",SYMTAB[SYMTABindex].LABEL,SYMTAB[SYMTABindex].address);
		SYMTABindex++;
	}
	fclose(f_symtab);
	printf("SYMTAB is written to symtab.txt file.\n");
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
	int SYMTABindex;
	readOPTAB();
	/*
		#PASS 1 algorithm starts from here
	*/
	/* check if the opcode of first line of the file is 'START' */
	if(strcmp(instructions[lineNumber].OPCODE,"START")==0){
		/* save #[operand] as starting address. The address given in the source file is by default taken as Hex and is converted to decimal. */ 
		sscanf(instructions[lineNumber].OPERAND,"%X",&STARTADR);	
		printf("STARTADR=%d <-(Decimal) %X <-(Hex)\n",STARTADR,STARTADR);
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
	
	/*while OPCODE !='END'*/
	while(strcmp(instructions[lineNumber].OPCODE,"END")!=0){
		/*if there a symbol in the LABEL field*/
		if(instructions[lineNumber].LABEL!=NULL){
			/*Search SYMTAB for LABEL. This function returns -1 if the LABEL is found, i.e. if there is an error. Else, it returns the index of the empty field in SYMTAB*/
			SYMTABindex=searchSYMTAB(lineNumber,instructions[lineNumber].LABEL);
			/*If error flag is set, then display error for duplicate labels*/
			if(SYMTABindex==-1){
				/*lineNumber started from 0. Hence lineNumber+1 is the appropriate lineNumber*/
				printf("\nERROR: Duplicate symbol '%s' found at line number %d\n",instructions[lineNumber].LABEL,lineNumber+1);	
			}
			else{
				/*insert (LABEL,LOCCTR) into SYMTAB*/
				SYMTAB[SYMTABindex].LABEL=strdup(instructions[lineNumber].LABEL);
				SYMTAB[SYMTABindex].address=LOCCTR;
			}
		}
		/*
			CONTINUE FROM HERE..... REMOVE UNNECESSARY LINES BELOW THIS COMMENT
			ALSO CHECK OUT FOR MEMORY LEAKS
		*/
		int found=searchOPTAB(instructions[lineNumber].OPCODE);
		//continue...
		if(found){
			printf("%s is present in the OPTAB\n",instructions[lineNumber].OPCODE);
		}else{
			printf("%s is NOT in the OPTAB\n",instructions[lineNumber].OPCODE);
		}
		lineNumber++;
		
	}
	writeSYMTAB();
	
}

void clearIntermediateFile(){
	FILE *f=fopen("intermediate_file.txt","w");
	fclose(f);
}

int main(){
	clearIntermediateFile();
	assembler_pass1();
	return 0;
}


