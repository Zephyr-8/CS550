

#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "cmdLine.h"
#include <string.h>
#include "error.h"
#include <errno.h>
#include <fcntl.h>

struct oprand {
    int oprand_index;
    int oprand_flag;//flag = 0 denoting a pipe
};

int terminal[2];//to save descriptor pointing to terminal

int analyze_command(char** tokens,int left,int right);
int redirection(char** tokens,int left,int right);

int main(void) {
    while (1){

        printf("mysh>");
        initCmdLine();
        struct command cline= getCmdLine();
        if(strcmp(cline.tokens[0],"exit")==0){
            freeCmdLine();
            exit(0);
        }
        if(strcmp(cline.tokens[0],"cd")==0){
            chdir(cline.tokens[1]);
            freeCmdLine();
        }
        else{
            int result =analyze_command(cline.tokens,0,cline.n);
            freeCmdLine();
            switch (result) {
                case ERROR_FORK:
                    fprintf(stderr, "\e[31;1mError: Fork error.\n\e[0m");
                    exit(ERROR_FORK);
                case ERROR_MANY_IN:
                    fprintf(stderr, "\e[31;1mError: Too many redirection symbol \"%s\".\n\e[0m", "<");
                    break;
                case ERROR_MANY_OUT:
                    fprintf(stderr, "\e[31;1mError: Too many redirection symbol \"%s\".\n\e[0m", ">");
                    break;
                case ERROR_INFILE_NOT_EXIST:
                    fprintf(stderr, "\e[31;1mError: Input redirection file not exist.\n\e[0m");
                    break;
                case ERROR_OUTFILE_NOT_EXIST:
                    fprintf(stderr, "\e[31;1mError: Output redirection file not exist.\n\e[0m");
                    break;
                case ERROR_MISS_PARAMETER:
                    fprintf(stderr, "\e[31;1mError: Miss redirect file parameters.\n\e[0m");
                    break;
                case ERROR_PIPE:
                    fprintf(stderr, "\e[31;1mError: Open pipe error.\n\e[0m");
                    break;
                case ERROR_PIPE_MISS_PARAMETER:
                    fprintf(stderr, "\e[31;1mError: Miss pipe parameters.\n\e[0m");
                    break;
            }
        }
    }
}

int analyze_command(char** tokens,int left,int right){
    if(left>=right) return RESULT_NORMAL;

    struct oprand op={-1,-1};
    for(int i=left;i<right;i++)
        if(strcmp(tokens[i],"|")==0){
            op.oprand_index=i;
            op.oprand_flag=0;
            break;
        }
    if (op.oprand_index==right-1){
        return  ERROR_PIPE_MISS_PARAMETER;
    }
    switch(op.oprand_flag){
        case(0): {
            int fds[2];
            if (pipe(fds) == -1) {
                return ERROR_PIPE;
            }
            int result = RESULT_NORMAL;

            pid_t pid = fork();
            if (pid == -1) {
                result = ERROR_FORK;
            }
            else if (pid == 0) {
                close(1);
                dup2(fds[1], STDOUT_FILENO);
                close(fds[0]);
                result = redirection(tokens,left,op.oprand_index);
                exit(result);
            }
            else {
                int status;
                waitpid(pid, &status, 0);
                if (op.oprand_index+1<right) {
                    close(0);
                    dup2(fds[0], STDIN_FILENO);
                    close(fds[1]);
                    result = analyze_command(tokens, op.oprand_index + 1, right);
                }
            }
            return result;
        }
        default:{
            int result=redirection(tokens,left,right);
            return result;
        }
    }
};



int redirection(char** tokens,int left,int right){
    int inNum = 0, outNum = 0;
    char *inFile = NULL, *outFile = NULL;
    int endIdx = right;

    for (int i=left; i<right; ++i) {
        if (strcmp(tokens[i], "<") == 0) { // input redirection
            ++inNum;
            if (i+1 < right)
                inFile = tokens[i+1];
            else return ERROR_MISS_PARAMETER;

            if (endIdx == right) endIdx = i;
        }
        else if (strcmp(tokens[i], ">") == 0) { // output redirection
            ++outNum;
            if (i+1 < right)
                outFile = tokens[i+1];
            else return ERROR_MISS_PARAMETER;

            if (endIdx == right) endIdx = i;
        }
    }
    if (inNum == 1) {
        FILE* fp = fopen(inFile, "r");
        if (fp == NULL)
            return ERROR_INFILE_NOT_EXIST;
        fclose(fp);
    }
    if (inNum > 1) { // too many inputs
        return ERROR_MANY_IN;
    }
    if (outNum == 1) {
        FILE* fp = fopen(outFile, "r");
        if (fp == NULL)
            return ERROR_OUTFILE_NOT_EXIST;
        fclose(fp);
    }
    else if (outNum > 1) { // too many outputs
        return ERROR_MANY_OUT;
    }
    int result = RESULT_NORMAL;
    pid_t pid = fork();
    if (pid == -1) {
        result = ERROR_FORK;
    }
    else if (pid == 0) {
        if (inNum == 1){
            freopen(inFile, "r", stdin);
        }
        if (outNum == 1){
            freopen(outFile, "w", stdout);
        }
        /* execution */
        char* comm[256];
        for (int i=left; i<endIdx; ++i){
            comm[i] = tokens[i];
        }
        comm[endIdx] = NULL;
        execvp(comm[left], comm+left);
        exit(errno);
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        int err = WEXITSTATUS(status); //
        if (err) {
            printf("\e[31;1mError: %s\n\e[0m", strerror(err));
        }
    }
    return result;
}