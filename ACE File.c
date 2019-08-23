/** Author: Stewart Laing, Reg no: 201711046
 * Author: Maximo Crichton-Smith, Reg no: 201740733
 * Author: Liam Wallace, Reg no: 201701137
 *
 *
 * Personal Statement: We confirm that this submission is all our own work.
 *
 * (Signed)Stewart Laing, Maximo Crichton-Smith, Liam Wallace
 *
 *
 *
 *
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#define lineSize 513
/*

*/
int tokeniser(char input[lineSize], char *tokenArray[50]);
void forkMethod(char *tokenArray[50], int command_flag);
int commandRunner(char input[lineSize], char *tokenArray[50], char history[20][lineSize], int store_index, char *aliasStorage[10][2]);
int cd(char *tokenArray[50]);
char *getHistory(int index, char history[20][lineSize]);
int execHistory(char *tokenArray[50], char history[20][lineSize], int store_index, char *aliasStorage[10][2]);
void setAlias(char *aliasStorage[10][2],char *tokenArray[50]);
void removeAlias(char *aliasStorage[10][2], char *tokenArray[50]);
void printAlias(char *aliasStorage[10][2]);
void saveHistory(char history[20][lineSize]);
int loadHistory(char history[20][lineSize]);
void saveAliases(char *aliasStorage[10][2]);
int setenv(const char *envname, const char *envval, int overwrite);
void setpath(char path[50]);
void loadAliases(char *aliasStorage[10][2], char *tokenArray[50]);

/*
The main method in our shell initialises all the values and arrays which are essential to our code and calls all the initial methods. These are loading in both the aliases and the history, getting and saving the inital path and changing the current working directory to HOME. The main method maintains a while loop to keep the code running until the user decides to exit, directing everthing to the right place until the method commandRunner returns -1. Finally the main runs all the final funtions such as saving both the aliases and history back to the files and resetting the path back to its initial.
*/

int main() {
	char input[lineSize] = { '\0' };
	char *tokenArray[50];
	int command_flag = 0;
	char path[513];
	char cwd[200];
	char history[20][lineSize];
	char *aliasStorage[10][2];

	printf("\n\033[1;34m{}{}{}{}{}{}{}{}{}{}{}{}\n\033[0m");
	printf("\033[1;34m{}{}{}{}{ACE  4}{}{}{}{} \n\033[0m");
	printf("\033[1;34m{}{}{}{}{}{}{}{}{}{}{}{} \n\n\033[0m");

	strcpy(path, getenv("PATH"));
	printf("%s \n", path);
	chdir(getenv("HOME"));
	getcwd(cwd, sizeof(cwd));
	int store_index = loadHistory(history);
	loadAliases(aliasStorage,tokenArray);
	printf("\033[1;32mWorking directory: %s \033[0m\n", cwd);
	printf(">>");

	// Command FLag:
	// -1 => exit value
	//  0 => Not a custom command
	//  1 => Custom command

	while (command_flag != -1 && fgets(input, 512, stdin) != NULL){
		if(input[0] != '!'){
			if(store_index == 20){
				for(int j=0; j<2; j++){
					for(int i=0; i<20; i++){
						strcpy(history[i], history[i+1]);
					}
				}
				store_index = 19;
			}
			else{
				store_index++;
			}
			strcpy(history[store_index], input);
		}

		if (strtok(input, "\n")) {
			int command_flag = commandRunner(input, tokenArray, history, store_index, aliasStorage);
			if (command_flag == -1) {
				chdir(getenv("HOME"));
				setpath(path);
				saveHistory(history);
				saveAliases(aliasStorage);
				exit(0);
			}
			forkMethod(tokenArray, command_flag);
		}

		getcwd(cwd, sizeof(cwd));
		printf("\033[1;32mWorking directory: %s \033[0m\n", cwd);
		printf(">>");
	}

	chdir(getenv("HOME"));
	setpath(path);
	saveHistory(history);
	saveAliases(aliasStorage);

	return 0;
}
/*
forKMethod creates child processes in order to be able to run system commands and exits the child after execution bringing the user back to the parent process. Furthermore this checks for invalid commands assuming a command doesn't exist in command runner and isn't and excectuable system command.
*/

void forkMethod(char *tokenArray[50], int command_flag) {
	pid_t pid;
	int status;

	pid = fork();
	if (pid < 0) {
		printf("fork failed \n");
	}
	else if (pid == 0) {
		pid = getpid();
		if (execvp(tokenArray[0], tokenArray)==-1 && command_flag == 0) {
			printf("Invalid command\n");
		}

		exit(0);
	} else {
		waitpid(pid, &status, 0);
	}
}

/*
tokeniser takes the users input line from the shell and splits it into its individual tokens, storing these in an array called tokenArray which allows for the individual tokens to be checked against expected inputs.
*/

int tokeniser(char input[lineSize], char *tokenArray[50]) {
	char *tok;
	int i = 0;
	int Case = 0;
	tok = strtok(input," <>|&\n\t\n");
	tokenArray[0] = tok;
		if ((strcmp(tok, "exit") == 0)) {
			Case = -1;
		}
		while(tok != NULL) {
			i++;
			tok = strtok(NULL, " <>|&\n\t\n");
			tokenArray[i] = tok;
			}
	return Case;
}

/*
commandRunner is essentially the main part of the code, every line entered by the user is first tokenised before it is brought into this method where the tokens are analysed and the correct function the user is asking for is called. It also contains a check to see if an alias appears after the first command which allows alias's to be called anywhere as well as allowing for the creation of advanced alias.
*/

int commandRunner(char input[lineSize], char *tokenArray[50], char history[20][lineSize], int store_index, char *aliasStorage[10][2]) {

	int command_flag = tokeniser(input, tokenArray);
	int counter = 0;
	int i;
	int a = 0;
	char alias[lineSize];
	char *search = tokenArray[a];

	while(tokenArray[counter] != NULL) {
		counter++;
	}

	if((strcmp(tokenArray[0], "alias") == 0) && (tokenArray[1] != NULL)) {
		command_flag = 1;
		setAlias(aliasStorage, tokenArray);
	}
	else if(strcmp(tokenArray[0], "unalias") == 0) {
		command_flag = 1;
		removeAlias(aliasStorage, tokenArray);
	}
	else if((strcmp(tokenArray[0], "alias") == 0) && (tokenArray[1] == NULL)) {
		command_flag = 1;
		printAlias(aliasStorage);
	}
	else {
		while (search != NULL){
		for(i = 0; i < 10; i++) {
			if(aliasStorage[i][0] != NULL) {
				strcpy(alias, aliasStorage[i][0]);
				if (strcmp(search, alias) == 0) {
					if (a == 0) {
						strcpy(input, aliasStorage[i][1]);
					}
					else {
						strcpy(input, tokenArray[0]);
						for (int j = 1; j < counter; j++) {
							strcat(input, " ");
							if(j == a) {
								strcat(input,aliasStorage[i][1]);
							} else {
								strcat(input,tokenArray[a]);
							}
							a++;
						}
					}
					for (int j = 1; j < counter; j++) {
						strcat(input," ");
						strcat(input,tokenArray[j]);

					}
					command_flag = tokeniser(input, tokenArray);
				}
			}
		}
		a++;
		search = tokenArray[a];
	}
	}
	if (strcmp(tokenArray[0],"setpath") == 0) {
		setpath(tokenArray[1]);
		command_flag = 1;
	}
	else if (strcmp(tokenArray[0],"getpath") == 0) {
		command_flag = 1;
		if (tokenArray[1] == NULL) {
			printf("%s\n", getenv("PATH"));
		}
		else {
			printf("Too many parameters for getpath, no parameters should follow \n");
		}
	}
	else if (strcmp(tokenArray[0], "cd") == 0) {
		command_flag = 1;
		cd(tokenArray);
	}
	else if (strcmp(tokenArray[0], "history") == 0 ||strcmp(tokenArray[0], "!") == 0 || strcmp(tokenArray[0], "!-") == 0 || strcmp(tokenArray[0], "!!") == 0){
			command_flag = execHistory(tokenArray, history, store_index, aliasStorage);
	}
	else if(strcspn(tokenArray[0],"!") == 0){
		command_flag = 1;
		printf("\033[1;31m'!' detected but history command is invalid. If arguments are supplied, a space is required after first parameter\n\033[0m");
	}

return command_flag;
}

/*
cd takes in the tokenArray and runs the expected result when the cd function is called by the user in the shell input, changing the directory.
*/

int cd(char *tokenArray[50]){
  if (tokenArray[1] == NULL) {
    chdir(getenv("HOME"));
  }
  else if (tokenArray[2] != NULL) {
	printf("Too many parameters, should be:\ncd 'location'\n");
	return 1;
  }
  else if (chdir(tokenArray[1]) == -1) {
      perror("This directory doesnt exist");
  }

  return 1;
}

/*
setPath takes in the path and and sets it to this
*/

void setpath(char path[50]) {
char temp[500];
	if (path == NULL) {
		printf("There are too few arguments, it should be:\nsetpath 'PATH' \n");
	}
	else {
		strcpy(temp, path);
		strcat(temp, ":");
		setenv("PATH", temp,1);
		printf("The current path is now... %s \n",getenv("PATH"));
	}
}

/*

*/

char *getHistory(int index, char history[20][lineSize]){
	//printf("Command at index %d is %s", index, history[index]);
	return history[index+1];
}

/*
	Either prints the history array, executes the command at the specified index or executes last command
*/
int execHistory(char *tokenArray[50], char history[20][lineSize], int store_index, char *aliasStorage[10][2]){
	char *ptr;
	int command_flag = 1;
		if(strcmp(tokenArray[0], "history")!=0){
			if(strcmp(tokenArray[0], "!") == 0 || strcmp(tokenArray[0], "!!") == 0 || strcmp(tokenArray[0], "!-")  == 0 ){
				int index;
				if(strcmp(tokenArray[0], "!!") != 0 && tokenArray[1] != NULL){
					index = strtol(tokenArray[1], &ptr, 10);
				}
				else if(tokenArray[1] == NULL){
					index = store_index-1;
				}
				if(strcmp(tokenArray[0], "!-") == 0){
						index = store_index - index;
				}

				if(strcmp(tokenArray[0],  "!" ) == 0){
					//index = index -1;
				}
				if(index > -1 && index < 21 && index < store_index){
					char historiccommand[lineSize];
					strcpy(historiccommand, getHistory(index, history));
					char *tokenisedcommand[50];
					tokeniser(historiccommand, tokenisedcommand);
					if(strcmp(tokenisedcommand[0],"cd") == 0){
						cd(tokenisedcommand);
					}
					if(strcmp(tokenisedcommand[0], "exit") == 0){
						command_flag = -1;
					}
					if(strcmp(tokenisedcommand[0],"history") == 0 || strcmp(tokenisedcommand[0],"!") == 0 || strcmp(tokenisedcommand[0],"!-") == 0 || strcmp(tokenisedcommand[0],"!!") == 0){
						execHistory(tokenisedcommand, history, store_index, aliasStorage);

					}
					else if((strcmp(tokenArray[0], "alias") == 0) && (tokenArray[1] != NULL)) {
						//setAlias(aliasStorage, tokenisedcommand);
					}
					else if(strcmp(tokenArray[0], "unalias") == 0) {
						//removeAlias(aliasStorage, tokenArray);
					}
					else if((strcmp(tokenArray[0], "alias") == 0) && (tokenArray[1] == NULL)) {
						//printAlias(aliasStorage);
					}
					else{
						forkMethod(tokenisedcommand, command_flag);
					}
				}
				else{
					printf("\033[1;31mThe index for the command history is out of bounds. Only values that have been recorded between 1 and 20 inclusive allowed.\n\033[0m");
				}
			}
			else{
				printf("\033[1;31mThe first parameter for the command history is incorrect. Only '!', '!-' and '!!' allowed. \n\033[0m");
			}
		}
		else if(tokenArray[1] != NULL){
			printf("\033[1;31mNo arguments expected after 'history'\n\033[0m");
		}
		else if(store_index == 0){
			printf("\033[1;31mNo history detected\n\033[0m");
		}
		else{
			for(int i=0; i <= store_index; i++){
				if(strcmp(history[i],"")){
					printf("%d. %s", i+1, history[i]);
				}
			}
		}
		return command_flag;
}


/*
When setAlias is called it firstly checks if the last element in the aliasStorage array is empty and if not it means that the max of 10 alias's has already been met, this is due to alias's being filled in order and also reformatted if one is removed. The user input is then checked to make sure there is not too little perameters and if not returns another error code through the variable found. Assuming there is enough parameters and there is space the command is concatenated into a variable called aliasCommand followed by a null character. The alias name is then copied into aliasStorage[x][0] (where x is the alias number) and alias command is copied into aliasStorage[x][1] before found is set printing off a success message. If the alias is found to be a duplicate it is overwritten and a message is printing telling the user this.
*/

void setAlias(char *aliasStorage[10][2], char *tokenArray[50]) {
	int found = 0;
	int counter = 0;
	int i = 0;
	char *alias = tokenArray[1];
	char aliasCommand[lineSize]= { '\0' };
	i = 0;

	if (aliasStorage[9][1] != NULL) {
		found = 4;
	}

	if (tokenArray[2] != NULL) {
			while(tokenArray[counter] != NULL) {
				counter++;
			}
			
			for (i = 2; i < counter; i++) {
				strcat(aliasCommand,tokenArray[i]);
				strcat(aliasCommand," ");
			}
			strcat(aliasCommand, "\0");

	} else{
		found = 2;
	}

	i = 0;
	while(found < 1) {
		if (aliasStorage[i][0] == NULL) {
			aliasStorage[i][0] = malloc(lineSize * sizeof(char));
			aliasStorage[i][1] = malloc(lineSize * sizeof(char));
			strcpy(aliasStorage[i][0], alias);
			strcpy(aliasStorage[i][1], aliasCommand);
			found = 1;
		}
		else if (strcmp(alias, aliasStorage[i][0]) == 0) {
			aliasStorage[i][0] = malloc(lineSize * sizeof(char));
			aliasStorage[i][1] = malloc(lineSize * sizeof(char));
			strcpy(aliasStorage[i][0], alias);
			strcpy(aliasStorage[i][1], aliasCommand);
			found = 3;
		}
		i++;
	}

	if(found == 1){
		printf("Alias %s was assigned to command '%s'.\n", alias, aliasCommand);
	}
	else if (found == 2) {
		printf("There are too few parameters to set this alias\n");
	}
	else if (found == 3) {
		printf("This alias already existed and therefore has been overwritten\n");
	}
	else if (found == 4) {
		printf("There are too many alias's already, please replace or unalias to add \n");
	}
	else{
		printf("Error: Critical alias fault.\n");
	}
}

/*
removerAlias firstly runs a counter to count the amount of alias's stored before a check is ran to make sure there are enough parameters for unalias to be executed. If not an error code results in a print to the user with some help. The aliasStorage array is then iterated through checking to see if the alias can be found and if not it sets a code causing a print to the user explaining this. If the alias is found then the command is removed by replacing the stored contents with "NULL" and every alias after is pushed back to keep the array from having any blank spaces between aliases.
*/

void removeAlias(char *aliasStorage[10][2], char *tokenArray[50]) {
	int counter = 0;
	int errorCode = 0;
	int i = 0;
	int exists = 0;
	
	while(aliasStorage[counter][0] != NULL) {
		counter++;
	}
	if (tokenArray[1] == NULL) {
		errorCode = 2;
	}
	else if (errorCode < 1) {
		for(i = 0; i <= counter; i++) {
			if (aliasStorage[i][0] != NULL) {
				if(strcmp(aliasStorage[i][0], tokenArray[1]) == 0){
					aliasStorage[i][0] = NULL;
					aliasStorage[i][1] = NULL;
					exists = 1;
				}
			}
		}
		while (aliasStorage[i][0] != NULL) {
			strcpy(aliasStorage[i][0], aliasStorage[i+1][0]);
			strcpy(aliasStorage[i][1], aliasStorage[i+1][1]);
		i++;
		}
	}

	
	if (errorCode == 2) {
		printf("There are too few parameters, should be :\nunalias 'alias name'\n");
	}
	else if(exists == 0) {
		printf("This command does not exist\n");
	}
}

/*
printAlias takes in the aliasStorage array which contains all the alias's and prints them off as a list. It also prints a message indicating if the array is empty.
*/

void printAlias(char *aliasStorage[10][2]) {
	printf("Alias set:\n");
	for (int i = 0; i < 10; i++) {
		if (aliasStorage[i][0] != NULL) {
			printf("Alias %s is %s \n", aliasStorage[i][0], aliasStorage[i][1]);
		}
	}
	if (aliasStorage[0][0] == NULL) {
		printf("There are no stored alias's currently\n");
	}
}
/*
loadHistory loads in the previous commands called by users during the last iteration from a  file called 'hist_list'. The commands are stored in the 2D array of char called history.
*/

int loadHistory(char history[20][lineSize]) {
	char line[lineSize];
	FILE * fptr;
	fptr = fopen(".hist_list","r");
	if (fptr == NULL) {
		perror("No file found for history : ");
	}
	else {
		int count = 0;
		while(fgets(line, lineSize, fptr)){
			strcpy(history[count], line);
				if(lineSize!=0){
					count++;
				}
		}
		fclose(fptr);
		return count-1;
	}
return 0;
}

/*
loadAliases loads in all the aliases from the alias text file. As the aliases are stored as commands we can simply tokenise the line in the text file then call the setAlias method which will essentially reinitialise all the stored alias's.
*/


void loadAliases(char *aliasStorage[10][2], char *tokenArray[50]){
	char line[lineSize];
	FILE *fptr;
	int i = 0;
	fptr = fopen(".alias","r");
	if (fptr == NULL) {
		perror("No file found for alias : ");
	}
	else {
		while(fgets(line, lineSize, fptr)){
			if(strcmp(line, "\0") != 0){
			tokeniser(line, tokenArray);
				if(strcmp(tokenArray[1],"(null)") != 0) {
					setAlias(aliasStorage, tokenArray);
				}
			}
			i++;
		}
		fclose(fptr);
	}
}

/*
saveHistory saves the commands stored in the history array into a text file called 'hist_list', where later it can be loaded back into the program during the next iteration. 
*/

void saveHistory(char history[20][lineSize]) {
	FILE *fptr;
	fptr = fopen(".hist_list","w");
	if (fptr == NULL) {
		perror("No file found for history : ");
	}
	else {
		printf("\n\n Saving history...\n");
		for(int i = 0; i < 20; i++){
			if (strcmp(history[i],"\0") != 0){
				fprintf(fptr, "%s", history[i]);
			}
		}
		fclose(fptr);
	}
}

/*
saveAliases saves the aliases stored in the alias storage array to a text file as commands
ie storing as "alias a b" which when loaded back in will re-initialise these aliases
*/

void saveAliases(char *aliasStorage[10][2]) {
	FILE *fptr;
	fptr = fopen(".alias", "w");
	if(fptr == NULL) {
		perror("No file found for alias : ");
	}
	else {
		printf("\n\n saving aliases...\n");
		for (int i = 0; i < 10; i++) {
			fprintf(fptr, "alias %s %s\n", aliasStorage[i][0], aliasStorage[i][1]);
		}
		fclose(fptr);
	}
}


////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
