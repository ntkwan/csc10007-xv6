#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    char buf[512]; // blank buffer 
    char *buf_ptr = buf; // pointer to the buffer char array
    char c[1]; // buffer for reading char in stdin


    char *args[MAXARG]; // list of arguments
    int nargs = argc - 1; // number of arguments
    
    for(int i = 0; i + 1 < argc; ++i) {
        args[i] = (char*)malloc(strlen(argv[i + 1]) + 1); // allocate memory for each argument in the args array 
        strcpy(args[i], argv[i + 1]); // copy the current argument from argv to args
    }

    while(read(0, c, 1) > 0) { // read next char from stdin
        if(*c == '\n') { // if the char is a newline
            
            *buf_ptr = '\0'; // reset pointer to null
            args[nargs] = (char *)malloc(strlen(buf) + 1);
            strcpy(args[nargs], buf);
            nargs++;

            int pid = fork();
            if(pid == 0) { // if this is the child
                if(exec(argv[1], args) < 0) { // execute the child failed
                    printf("xargs: exec %s failed\n", argv[1]);
                }
            } else { // if this is the parent
                wait(0);

                for(int i = argc - 1; i < nargs; ++i) { // free the memory of args array, to be clear that nargs has been updated
                    free(args[i]);
                }

                nargs = argc - 1; // reset nargs to the original value
                buf_ptr = buf; // reset to blank buffer
            }
        } else if(*c == ' ') { // if the char is a space (separator between arguments)
            *buf_ptr = '\0'; // reset pointer to null
            args[nargs] = (char *)malloc(strlen(buf) + 1);
            strcpy(args[nargs], buf);
            nargs++;

            buf_ptr = buf; // reset to blank buffer
        } else { // if the char is a regular char
            *buf_ptr = *c; // assign the char to the current position in the buffer through the pointer
            buf_ptr++; // move the pointer to the next position
        }
    }

    for(int i = 0; i + 1 < argc; ++i) {
        free(args[i]);
    }

    return 0;
}