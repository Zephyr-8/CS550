//
// Created by Tianze_Liu on 2/18/20.
//

#include "cmdLine.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
/* Global Variables */
char *cmdbuf=NULL;
size_t cmdbufsize=0;
int numTokens=32;
struct command cline={NULL,0};

void initCmdLine() {
    cline.tokens=malloc(sizeof(char *) * numTokens);
}

struct command getCmdLine() {
    // getline will reallocate cmdbuf to be large enough to fit the next line from stdin
    if (getline(&cmdbuf,&cmdbufsize,stdin)<0) {
        if (feof(stdin)) {
            cline.tokens[0]="exit";
            cline.tokens[1]=NULL;
            return cline;
        }
        perror("getCmdLine invocation of getline: ");
        cline.tokens[0]=NULL;
        return cline;
    }
    // Break up cmd based on white space
    int n=0;
    char *this_token;
    while( (this_token= strsep(&cmdbuf, " \t\v\f\n\r")) !=NULL) {
        if (*this_token=='\0') continue;
        cline.tokens[n]=this_token;
        n++;
        if (n>=numTokens) { // increase the size of the tokens
            numTokens *=2; // Double the size
            assert( (cline.tokens = realloc(cline.tokens,sizeof(char *) * numTokens)) != NULL);
        }
    }
    cline.tokens[n]=NULL;
    cline.n=n;
    return cline;
}

void freeCmdLine() {
    free(cline.tokens);
    free(cmdbuf);
}