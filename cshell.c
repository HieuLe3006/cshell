#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define BUFF_SIZE 1024
#define TRUE 1
#define FALSE 0
#define MAX_LETTERS 100
#define MAX_COMMANDS 100

typedef struct{
	char *name;
	struct tm *time;
	int code;
}Command;

typedef struct{
	char* name;
	char* value;
}EnvVar;

//function to parse tokens
void parse_tokens(char **token, char *CommandLine){
	int index = 0;
	token[index] = strsep(&CommandLine, " ");
	while(token[index]!=NULL){
		index++;
		token[index] = strsep(&CommandLine, " ");
	}
}

//built-in function handler
void builtinCommandHandler(){
	char* ListOfCommands[4];

	ListOfCommands[0] = "exit";
	ListOfCommands[1] = "print";
	ListOfCommands[2] = "log";
	ListOfCommands[3] = "theme";
}

//function to store environment variables
void variableStoring(char** token, EnvVar* variables){
	for(int i = 0; i<MAX_COMMANDS; i++){
		variables[i].name = malloc(BUFF_SIZE*sizeof(char));
		variables[i].value = malloc(BUFF_SIZE*sizeof(char));
		if(token[i]==NULL)
			break;

		//Check if a command is a variable declaration
		if(token[i][0]=='$'){
			int j;

			//Getting the name
			for(j = 1; j<strlen(token[i]); j++){
				if(token[i][j]=='=')
					break;
				variables[i].name[j-1] = token[i][j];
			}

			//Getting the value of variable
			int k = 0;
			while(j<strlen(token[i])){
				j++;
				variables[i].value[k] = token[i][j];
				k++; 
			}
		}
	}
}

//function to check return code
int checkReturnCode(Command theCommand){
	return 0;
}

//function to store log info
void logStoring(char** token, Command* logs, int* index){
	//check if command is environmental variable declaration;
	if(token[0][0] == '$'){
		int i = 0;
		while(token[i]!=NULL){
			//Geting name
			logs[*index].name = malloc(BUFF_SIZE*sizeof(char));
			strcpy(logs[*index].name, token[i]);

			//Acquiring time of input command
			time_t rawtime;
			time(&rawtime);
			logs[*index].time = localtime(&rawtime);

			//Getting return code
			logs[*index].code = checkReturnCode(logs[*index]);
			i++;
			(*index)++;
		}
	}
	else{//If input command is not an environmental variable declaration
		//Getting name
		printf("The loop");
		logs[*index].name = malloc(BUFF_SIZE*sizeof(char));
		strcpy(logs[*index].name, token[0]);

		//Acquiring time of input command
		time_t rawtime;
		time(&rawtime);
		logs[*index].time = localtime(&rawtime);

		//Getting return code
		logs[*index].code = checkReturnCode(logs[*index]);
		printf("End of loop\n");
		(*index)++;
	}
}



int main(int argc, char **argv){
	int fds[2];
	char* CommandLine = malloc(BUFF_SIZE*sizeof(char));	//command line
	char *token[MAX_COMMANDS];			 				//token
	EnvVar variables[MAX_COMMANDS];
	Command logs[MAX_COMMANDS];
	int numberOfCommands = 0;

	//create Pipe
	if(pipe(fds)==-1){
		perror("Pipe");
		return(-1);
	}

	// Loop to read a line.
	while(TRUE){
		//Getting command line
		fgets(CommandLine, BUFF_SIZE, stdin);
		if(strcmp(CommandLine, "")==0)
			break;


		//terminate if command is "exit"
		if(strcmp(CommandLine, "exit\n")==0)
			break;

		//splitting tokens
		parse_tokens(token, CommandLine);

		//Storing environment variables
		variableStoring(token, variables);

		//Logging input commands
		logStoring(token, logs, &numberOfCommands);
		printf("new print\n");
		for(int i = 0; i<numberOfCommands; i++){
			if(logs[i].name==NULL)
				break;
			printf("%s%s %d\n", asctime(logs[i].time), logs[i].name, logs[i].code);
		}

	}
	return 0;
}