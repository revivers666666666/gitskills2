#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MYSH_TOK_DELIM "\t\r\n"
#define MYSH_TOK_BUFFER_SIZE 64
#define MAX_HISTORY_SIZE 100
#define MAX_INPUT_SIZE 1024

int mysh_cd(char **args);
int mysh_help(char **args);
int mysh_exit(char **args);
int mysh_builtin_nums();
int mysh_history(char **args);
int history_count,i,j;
char *history[MAX_HISTORY_SIZE];

int add_to_history(char **args){
    if(history_count < MAX_HISTORY_SIZE)
   {
    history[history_count]=strdup(args[0]);
    history_count++;
}else{/*挪移剩余元素，去除首个元素*/
    free(history[0]);
    for(i=1;i<MAX_HISTORY_SIZE;i++){
        history[i-1]=history[i];
    }
/*再把新指令输入*/history[MAX_HISTORY_SIZE-1] =strdup(args[0]);
}
return 1;
}


int mysh_history(char **args) {
    for (int j = 0; j < history_count; j++) {
        printf("%d: %s\n",  j + 1, history[j]);
    }
    return 1;
}


//内置程序地址存储数组
int (*builtin_func[])(char **) =
{
    &mysh_cd,
    &mysh_help,
    &mysh_exit,
    &mysh_history
};

//cd 命令
int mysh_cd(char **args)
{
    if(args[1] == NULL)
    {
        perror("Mysh error at cd,lack of args\n");
    }
    else
    {
        if(chdir(args[1]) != 0)
        perror("Mysh error at chdir\n");
    }
    return 1;
};

char * builtin_cmd[] =
{
    "cd",
    "help",
    "exit",
    "history"
};

int mysh_help(char **args)
{
    puts("This is Dou's shell");
    puts("Here are some built in cmd:");
    for(int i = 0; i < mysh_builtin_nums(); i ++){
        printf("%s\n", builtin_cmd[i]);
        
    }
    return 1;
};

int mysh_exit(char **args){
    return 0;
}



int mysh_builtin_nums(){
    return sizeof(builtin_cmd) / sizeof(builtin_cmd[0]);
};

char * mysh_read_line()
{
    char * line = NULL;
    ssize_t bufsize;
    getline(&line, &bufsize,stdin);
    return line;
}
char** mysh_split_line(char * line){
    int buffer_size = MYSH_TOK_BUFFER_SIZE ,position = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;
    token = strtok(line,MYSH_TOK_DELIM);
    while(token != NULL)
    {
        tokens[position++] = token;
        token = strtok(NULL,MYSH_TOK_DELIM);
    }//第一次分词后，原本检测出的变为空
    tokens[position] = NULL;
    return tokens;
}

int mysh_launch(char **args)
{
    pid_t pid,wpid;
    int status;

    pid = fork();
    if(pid == 0){
        if(execvp(args[0],args) == -1)
         perror("Mysh error at execvp\n");
         exit(EXIT_FAILURE);
    }
    else{
        do
        {
            wpid = waitpid(pid,&status,WUNTRACED);
        } while (!WIFEXITED(status)  && !WIFSIGNALED(status)            
        );
    }
    return 1;
}
int mysh_execute(char **args)
{
    if(args[0] == NULL) return 1;
    for(int i=0;i < mysh_builtin_nums(); i ++){
        if(strcmp(args[0], builtin_cmd[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }
    return mysh_launch(args);


}


void mysh_loop()
{
    char *line;
    char **args;
    int status;
    do
    {
        char path[100];
        getcwd(path,100);
        char now[200] = "[mysh";
        strcat(now ,path);
        strcat(now,"]$");
        printf("%s",now);

        line = mysh_read_line();
        args = mysh_split_line(line);
        status = mysh_execute(args);
        add_to_history(args);
        free(line);
        free(args);
    }while(status);
}

int main() {
    mysh_loop();
    return 0;
}