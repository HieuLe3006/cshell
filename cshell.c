#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>

#define BUFF_SIZE 1024
#define TRUE 1
#define FALSE 0
#define MAX_LETTERS 100
#define MAX_COMMANDS 20
#define COLOR_WHITE "\x1b[0m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"

typedef struct{
	char *name;
	struct tm *time;
	int code;
}Command;

typedef struct{
	char* name;
	char* value;
}EnvVar;

char* ListOfCommands[4];
EnvVar *variables[MAX_COMMANDS];
Command *logs[MAX_COMMANDS];

int numberOfCommands = 0;
int numberOfVariables = 0;

bool interactive = TRUE;

//function to parse tokens
int parse_tokens(char **token, char *CommandLine){
	// printf("This is the parsing tokens\n");
	int index = 0;
	int num = -1;
	token[index] = strsep(&CommandLine, " ");
	while(token[index]!=NULL){
		num++;
		index++;
		token[index] = strsep(&CommandLine, " ");
	}
	int length = strlen(token[index-1]); 
	if(token[index-1][length-1]=='\n'){
		token[index-1][length-1] = '\0';
	}
	// printf("Printing tokens: ");
	// for(int i = 0; i<num+1; i++){
	// 	printf("%s ", token[i]);
	// }
	// printf("\n");
	return num;
}

//check if built-in command
bool checkBuiltIn(char* command){
	for(int i = 0; i<4; i++){
		if(strcmp(command, ListOfCommands[i])==0){
			return TRUE;
		}
	};
	return FALSE;
}

//built-in function handler
int builtinCommandHandler(char** token){
	// ListOfCommands[0] = "theme";
	// ListOfCommands[1] = "log";
	// ListOfCommands[2] = "print";
	// ListOfCommands[3] = "exit";


	//if command == theme
	if(strcmp(token[0], ListOfCommands[0])==0){
		if(token[1] == NULL){
			return 1;
		}
		else{
			if(strcmp(token[1], "green")==0){
				printf(COLOR_GREEN);
				return 0;
			}
			else if(strcmp(token[1], "red")==0){
				printf(COLOR_RED);
				return 0;
			}
			else if(strcmp(token[1], "blue")==0){
				printf(COLOR_BLUE);
				return 0;
			}
			else if(strcmp(token[1], "yellow")==0){
				printf(COLOR_YELLOW);
				return 0;
			}
			else if(strcmp(token[1], "white")==0){
				printf(COLOR_WHITE);
				return 0;
			}
			else{
				return 1;
			}
		}
	}
	else if(strcmp(token[0], ListOfCommands[1])==0){ //if command == log
		if(numberOfCommands==0){
			printf("\n");
		}
		else{
			for(int i = 0; i<numberOfCommands; i++){
			printf("%s %s %d\n", asctime(logs[i]->time), logs[i]->name, logs[i]->code);
			}
		}
		return 0;
	}
	else if(strcmp(token[0], ListOfCommands[2])==0){ //if command == print
		//check if theres anything to print
		if(token[1]==NULL){
			return 1;
		}
		else{
			// checking if user wants to print the value of a variable
			if(token[1][0]=='$'){
				if(numberOfVariables==0){
					return 1;
				}

				//check if the variable exists
				bool exist = FALSE;
				int i;
				for(i = 0; i<numberOfVariables; i++){
					if(strcmp(variables[i]->name, token[1])==0){
						exist = TRUE;
						break;
					}
				}

				if(exist == FALSE){

					return 1;
				}
				else{
					printf("%s\n", variables[i]->value);
					return 0;
				}
			}
			else{
				for(int i = 1; i<numberOfCommands; i++){
					printf("%s\n %s %d\n", asctime(logs[i]->time), logs[i]->name, logs[i]->code);
				}
				return 0;
			}
		}
	}
	else{ //if command == exit
		exit(0);
	}


}

//check for variable declaration
bool checkVar(char* command){
	if(command[0]=='$'){
		// printf("Return TRUE\n");
		return TRUE;
	}
	return FALSE;
}

//check if there is already a version of the variable recorded
bool checkExist(char* command){
	if(numberOfVariables==0){
		// printf("Number of varibles == 0\n");
		return FALSE;
	}
	else{
		for(int i = 0; i<numberOfVariables; i++){
			if(strcmp(command, variables[i]->name)==0)
				return TRUE;	
		}
		return FALSE;
	}
}

//function to store environment variables
int variableStoring(char** token){
	if(token[1]!=NULL){
		printf("Syntax Error\n");
		return 1;
	}
	else{
		//check for "="
		bool answer = FALSE;
		for(int i = 0; i<strlen(token[0]); i++){
			if(token[0][i]=='='){
				answer = TRUE;
				break;
			}
		}
		if(answer!=TRUE){
			printf("Syntax Error\n");
			return 1;
		}
		else{
			// printf("Syntax not error\n");

			//getting the size of the name
			int trace = 0;
			while(token[0][trace]!='=' && trace<strlen(token[0])){
				trace++;
			}

			//check for syntax eligibility
			if(trace==strlen(token[0])){
				printf("Syntax Error\n");
				return 1;
			}

			//extract the name of the variable
			char name[trace+1];
			for(int i = 0; i<trace; i++){
				name[i] = token[0][i];
			}
			name[trace] = '\0';

			int i = 0;
			int j = 0;

			//getting the name of the variable
			while(token[0][i]!='='){
				//variables[numberOfVariables]->name[j] = token[0][i];
			i++;
			}

			//getting the value of the variable
			i++;
			int index = i;
			while(token[0][i]!='\0'){
				i++;
				j++;
			}
			char value[j+1];
			for(int k = 0; k<j; k++){
				value[k] = token[0][index];
				index++;
			}
			value[j] = '\0';

			//check if variable has existed
			if(checkExist(name)){
				//find the variable to update value
				int m;
				for(m = 0; m<numberOfVariables; m++){
					if(strcmp(name, variables[m]->name)==0){
						break;
					}
				}

				//updating 
				strcpy(variables[m]->value, value);
			}
			else{
				variables[numberOfVariables] = (EnvVar*)(malloc(sizeof(EnvVar)));
				//printf("No variables with the same name available %d\n", numberOfVariables);
				variables[numberOfVariables]->name = (char*)(malloc(sizeof(char)));
				variables[numberOfVariables]->value = (char*)(malloc(sizeof(char)));

				strcpy(variables[numberOfVariables]->name, name);

				strcpy(variables[numberOfVariables]->value, value);
				numberOfVariables++;
			}
			return 0;
		}
			
	}
}

//function to store log info
int logStoring(char** token, Command** logs, int returnCode){
	//printf("The command to copy is: %s\n", token[0]);
	//logging the command
	logs[numberOfCommands] = (Command*)(malloc(sizeof(Command)));
	logs[numberOfCommands]->name = (char*)(malloc(sizeof(char)));

	//Getting the command
	strcpy((logs[numberOfCommands]->name), token[0]);

	//Acquiring time of input 
	time_t rawtime;
	time(&rawtime);
	logs[numberOfCommands]->time = localtime(&rawtime);

	//Getting return code
	logs[numberOfCommands]->code = returnCode;

	numberOfCommands++;
	return 0;
}


//Execute normal cmd commands
int executeNormal(char** token){
	pid_t pid = fork();

	if(pid == -1){
		perror("fork");
		return 1;
	}

	//child process
	if(pid==0){
		execvp(token[0], token);
		return 1;
	}
	else{ //parent process
		int childstatus;
		waitpid(pid, &childstatus, 0);
		return 0;
	}
}

//Execute pipe commands
int executePipe(char** arg1, char**arg2){
	int fds[2];
	pipe(fds);
	int i;
	pid_t pid = fork();

	if(pid==-1){
		perror("fork");
		return 1;
	}

	//child process
	if(pid == 0){
		close(fds[1]);
		dup2(fds[0],0);
		execvp(arg2[0], arg2);
	}
	else{//parent process
		close(fds[0]);
		dup2(fds[1], 1);
		execvp(arg1[0], arg1);
		return 1;
	}
}

//Get the name of the script



int main(int argc, char **argv){
	char* CommandLine = (char*)(malloc(sizeof(char)));	//command line
	char *token[MAX_COMMANDS];							//token	
	char *arg1[MAX_COMMANDS] = {0}; 		
	char *arg2[MAX_COMMANDS] = {0};		

	int num, k, returnCode;

	returnCode = 0;

	bool checkPipe = FALSE;

	ListOfCommands[0] = "theme";
	ListOfCommands[1] = "log";
	ListOfCommands[2] = "print";
	ListOfCommands[3] = "exit";

	for(int i = 0; i<MAX_COMMANDS; i++){
		token[i] = (char*)malloc((MAX_COMMANDS+1)*sizeof(char));
	}

	char* nameOfFile = (char*)malloc(sizeof(char));

	FILE* openReadFile;

	//check if the program is in script mode
	if(argv[1]==NULL){
		//printf("Interactive mode\n");
	}
	else{	
		interactive = FALSE;
		//printf("Script mode\n");

		//get the name of the file
		strcpy(nameOfFile, argv[1]);
		strcat(nameOfFile, ".txt");

		openReadFile = fopen(nameOfFile, "rb");
		if(access(nameOfFile, F_OK)==0){
			//printf("File exists\n");
		}
		else{
			//printf("File does not exist\n");
			return 0;
		}

	}

	int count = 0;

	// Loop to read a line.
	// For interactive mode
	while(TRUE){
		count++;
		if(interactive==TRUE){
			//printf("Reading line in interactive mode\n");
			fgets(CommandLine, 100, stdin);
			if(strcmp(CommandLine, "")==0)
				break;
		}
		else{
			// fseek(openReadFile, 0, SEEK_END);
			// int size = ftell(openReadFile);
			// printf("%d\n", size);
			// if(0==size){
			// 	break;
			// }
			// else{
			// 	printf("Getting line\n");
			// 	fgets(CommandLine, 100, openReadFile);
			// 	printf("%s\n", CommandLine);
			// 	if(EOF){
			// 		break;
			// 	}
			// }
			if(fgets(CommandLine, 100, openReadFile)==NULL){
				return 0;
			}
		}

		// splitting tokens
		num = parse_tokens(token, CommandLine);

		// check if there are built-in commands
		if(checkBuiltIn(token[0])==TRUE){
			returnCode = builtinCommandHandler(token);

			logStoring(token, logs, returnCode);

			// if(returnCode==1){ // exit if input command is "exit"
			// 	break;
			// }
		}
		else{
			// check for variable declaration
			if(checkVar(token[0])){
				returnCode = variableStoring(token);
				logStoring(token, logs, returnCode);
				// if(returnCode==1)
				// 	break;
			}
			else{
				// check if there is pipe.
				for(k = 0; k<num; k++){
					if(strcmp(token[k], "|")==0){
						checkPipe = TRUE;
						break;
					}
				}

				// //Storing environment variables
				// variableStoring(token, variables);

				if(checkPipe==TRUE){
					int x=0;
					while(x<k){ // copying params up to "|"
						arg1[x] = token[x];
						x++;
					}
					for(int i = 0; x<num; i++){
						arg2[i] = token[x];
						x++;
					}
				
					returnCode = executePipe(arg1, arg2);
					logStoring(token, logs, returnCode);
					// if(returnCode == 0)
					// 	break;
				}
				else{
					returnCode = executeNormal(token);
					logStoring(token, logs, returnCode);
					// if (returnCode==0)
					// 	break;
				}
			}

		}

		num = 0;
		checkPipe = FALSE;
		if(interactive==TRUE){
			fflush(stdin);
		}
		else{
			if(returnCode==1){
				printf("Missing keyword or command, or permission problem\nBye!");
				break;
			}
		}
	}

	for(int i = 0; i<num+1; i++){
		free(token[i]);
	}
	//free(token);
	if(interactive==FALSE){
		fclose(openReadFile);
	}
	
	return 0;
}