#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/signal.h>
#define true 1
#define false 0
void myWork(char command[]);
void executioner(char* args[]);
void cd(char* args[]);
void pauseShell(char* args[]);
void clr(char* args[]);
void dir(char* args[]);
void environ(char* args[]);
void myExecutionar(char* args[]);
void takeCommandFromAFile(char* args[]);
void I_O_Redirection(char* args[]);
void openMyManual(char* args[]);
char* workingDirectoryPath;
int isbackGround = false;
int outputRedirection = false;
int inputRedirection = false;
int inputAndOutputRedirection = false;
int indexForInputRedirection;
int isAppend = false;
int indexForOutputRedirection;
void myWork(char command[]){
    char *args[11];
    char *token = strtok(command, " ");
    int numOfArgs = 0;
    int indexForBG;
    while (token != NULL && numOfArgs < 10){
        args[numOfArgs] = token;
        if(!strcmp(args[numOfArgs],"&")){
            isbackGround = true;
            indexForBG = numOfArgs;
        }
        token = strtok(NULL, " ");
        numOfArgs++;
    }
    args[numOfArgs] = NULL;
    if(isbackGround){
        indexForBG++;
        if(args[indexForBG] != NULL){
            printf("Error Writing & please try again :(");
            return;
        }
        indexForBG--;
        args[indexForBG] = NULL;
    }
    char* tok = args[0];
    int count = 0;
    while(tok != NULL){
        if(!strcmp(args[count],">")){
            if(inputRedirection == true){
                inputRedirection = false;
                inputAndOutputRedirection = true;
            }
            else{
                outputRedirection = true;
            }
            indexForOutputRedirection = count;    
        }
        else if(!strcmp(args[count],"<")){
            if(outputRedirection == true){
                outputRedirection = false;
                inputAndOutputRedirection = true;
            }
            else{
                inputRedirection  =  true;
            }
            indexForInputRedirection = count;    
        }
        else if(!strcmp(args[count],">>")){
            if(inputRedirection == true){
                inputRedirection = false;
                inputAndOutputRedirection = true;
            }
            else{
                outputRedirection = true;
                isAppend = true;
            }
            indexForOutputRedirection = count;
        }    
        count++;
        tok = args[count];
    }
    if(inputRedirection || outputRedirection || inputAndOutputRedirection){
        I_O_Redirection(args);
        return;
    }
    executioner(args);
}
void executioner(char* args[]){
    if(args[0] == NULL);
    else if(!strcmp(args[0],"cd")){
        cd(args);
    }
    else if(!strcmp(args[0],"clr")){
        clr(args);
    }
    else if(!strcmp(args[0],"dir")){
        dir(args);
    }
    else if(!strcmp(args[0],"environ")){
        environ(args);
    }
    else if(!strcmp(args[0],"pause")){
        pauseShell(args);
    }
    else if(!strcmp(args[0],"quit")){
        exit(0);
    }
    else if(!strcmp(args[0],"myShell")){
        takeCommandFromAFile(args);
    }
    else if(!strcmp(args[0],"help")){
        openMyManual(args);
    }
    else{
        myExecutionar(args);
    }
}
void cd(char* args[]){
    if (args[1] == NULL){
        printf("\033[0;35m %s\n \033[0m",workingDirectoryPath);
    }
    else{
        if (chdir(args[1]) == -1){
            fprintf(stderr, "cd error: %s\n",strerror(errno));
        }
        else{
            getcwd(workingDirectoryPath, 50);
            setenv("PWD",workingDirectoryPath,1);
        }
    }
}
void pauseShell(char* args[]){
    while (getchar() != '\n');
}
void clr(char* args[]){
    pid_t child = fork();
    if(child == 0){
       if(execl("/usr/bin/clear","/usr/bin/clear",NULL) == -1){
            fprintf(stderr, "Error: %s\n",strerror(errno));
            exit(0);
        }
    }
    else{
        if(isbackGround){
            isbackGround = 0;
        }
        else{
            waitpid(child,NULL,0);
        }
    }
}
void dir(char* args[]){
    pid_t child = fork();
    if(child == 0){
        int c = execvp(args[0],args);
        if(c < 0){
            fprintf(stderr, "Error: %s\n",strerror(errno));
            exit(0);
        }
    }
    else{
        if(isbackGround){
            isbackGround = false;
        }
        else{
            waitpid(child,NULL,0);
        }
    }
}
void environ(char* args[]){
    args[0] = "env";
    pid_t child = fork();
    if(child == 0){
        int c = execvp(args[0],args);
        if(c < 0){
            fprintf(stderr, "Error: %s\n",strerror(errno));
            exit(0);
        }
    }
    else{
        if(isbackGround){
            isbackGround = false;
        }
        else{
            waitpid(child,NULL,0);
        }
    }
}
void myExecutionar(char* args[]){
    pid_t child = fork();
    if(child == 0){
        if(execvp(args[0],args) == -1){
            fprintf(stderr, "Error: %s\n",strerror(errno));
            exit(0);
        }
    }
    else{
        if(isbackGround){
            isbackGround = false;
        }
        else{
            waitpid(child,NULL,0);
        }
    }
}
void takeCommandFromAFile(char* args[]){
    char* line = (char*) malloc(100 * sizeof(char));
    size_t c;
    size_t max_load = 100;
    FILE* fd = fopen(args[1],"r");
    if(fd == NULL){
        fprintf(stderr, "Error: %s\n",strerror(errno));
        return;
    }
    while(c = getline(&line,&max_load,fd) != -1){
        line = strtok(line,"\n");
        if(line == NULL){
           continue;
        }
        myWork(line);
   }
}
void I_O_Redirection(char* args[]){
    FILE* fd;
    if(inputRedirection == true && outputRedirection == false){
        int temp_index = indexForInputRedirection;
        temp_index++;
        char* pathForMyInputFile = args[temp_index];
        args[indexForInputRedirection] = NULL;
        pid_t child = fork();
        if(child < 0){
            fprintf(stderr, "Error: %s\n",strerror(errno));
        }
        else if(child == 0){
            FILE* fd = fopen(pathForMyInputFile,"r");
            int fileNo = fileno(fd);
            dup2(fileNo,0);
            executioner(args);
            exit(0);
        }
        else{
            inputRedirection = false;
            waitpid(child,NULL,0);
        }
    }
    else if(outputRedirection == true && inputAndOutputRedirection == false){
        int temp_index = indexForOutputRedirection;
        temp_index++;
        char* pathForMyOutputFile = args[temp_index];
        args[indexForOutputRedirection] = NULL;
        if(isAppend == false){
            pid_t child = fork();
            if(child < 0){
                fprintf(stderr, "Error: %s\n",strerror(errno));
            }
            else if(child == 0){
                fd = fopen(pathForMyOutputFile,"w");
                if(fd == NULL){
                    fprintf(stderr, "Error: %s\n",strerror(errno));
                    exit(0);
                }  
                int fileNo = fileno(fd);
                dup2(fileNo,1);
                executioner(args);
                exit(0);
            }
            else{
                isAppend = false;
                outputRedirection = false;
                waitpid(child,NULL,0);
            }
        }
        else if(isAppend == true){
            pid_t child = fork();
            if(child < 0){
                fprintf(stderr, "Error: %s\n",strerror(errno));
            }
            else if(child == 0){
                fd = fopen(pathForMyOutputFile,"a");
                if(fd == NULL){
                    fprintf(stderr, "Error: %s\n",strerror(errno));
                    exit(0);
                }  
                int fileNo = fileno(fd);
                dup2(fileNo,1);
                executioner(args);
                exit(0);
            }
            else{
                outputRedirection = false;
                waitpid(child,NULL,0);
            }
        }  
    }
    else if(inputAndOutputRedirection == true){
        int min;
        if(indexForOutputRedirection > indexForInputRedirection){
            min = indexForInputRedirection;
        }
        else{
            min = indexForOutputRedirection;
        }
        indexForInputRedirection++;
        char* pathForMyInputFile = args[indexForInputRedirection];
        indexForOutputRedirection++;
        char* pathForMyOutputFile = args[indexForOutputRedirection];
        args[min] = NULL;
        pid_t child = fork();
        if ((child < 0)){
            fprintf(stderr, "Error1: %s\n",strerror(errno));
            exit(0);
        }
        else if(child == 0){
            if(isAppend == true){
                fd = fopen(pathForMyOutputFile,"a");
                if(fd == NULL){
                    fprintf(stderr, "Error2: %s\n",strerror(errno));
                    exit(0);
                }  
                int fileNo = fileno(fd);
                dup2(fileNo,1);
            }
            else if(isAppend == false){
                fd = fopen(pathForMyOutputFile,"w");
                if(fd == NULL){
                    fprintf(stderr, "Error3: %s\n",strerror(errno));
                    exit(0);
                }  
                int fileNo = fileno(fd);
                dup2(fileNo,1);
            }
            FILE* fd2 = fopen(pathForMyInputFile,"r");
            if(fd == NULL){
                fprintf(stderr, "Error4: %s\n",strerror(errno));
                exit(0);
            }
            int fileNo2 = fileno(fd2); 
            dup2(fileNo2,0); 
            executioner(args);
            exit(0);
        }
        else{
            inputAndOutputRedirection = false;
            inputRedirection = false;
            outputRedirection = false;
            waitpid(child,NULL,0);
        }
    }   
}
void openMyManual(char* args[]){    
    pid_t child = fork();
    if(child == 0){
        int c =  execl("/usr/bin/more","/usr/bin/more",
        "/home/mahmoud/Desktop/Jaafar/Project/myManual.txt",NULL);
        if(c == -1){
            fprintf(stderr, "Error: %s\n",strerror(errno));
            exit(0);
        }
    }
    else{
        if(isbackGround == true){
            isbackGround = false;
        }
        else{
            waitpid(child,NULL,0);
        }
    }

}

void main(int argc,char* argv[]){
    
    size_t sizeOfPath = 100;
    workingDirectoryPath = (char *)malloc(100 * sizeof(char));
    getcwd(workingDirectoryPath,sizeOfPath);
    chdir(workingDirectoryPath);
    //setenv("PWD",workingDirectoryPath,1);
    int size = 100;
    while(true){
        printf("\033[1;32m%s\033[0m","Jaafar@Jaafar:");
        printf("\033[1;34m ~%s> \033[0m", workingDirectoryPath);
        char command[size];
        scanf("%[^\n]99s", command);
        getchar();
        myWork(command);
        memset(command, 0x00, sizeof(command[0]) * size); 
    }
}


