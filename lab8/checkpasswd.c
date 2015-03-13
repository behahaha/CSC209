#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Read a user id and password from standard input, 
   - create a new process to run the validate program
   - use exec (probably execlp) to load the validate program.
   - send 'validate' the user id and password on a pipe, 
   - print a message 
        "Password verified" if the user id and password matched, 
        "Invalid password", or 
        "No such user"
     depending on the return value of 'validate'.

Setting the character arrays to have a capacity of 256 when we are only
expecting to get 10 bytes in each is a cheesy way of preventing most
overflow problems.
*/

#define MAXLINE 256
#define MAXPASSWD 10

void strip(char *str, int capacity) {
    char *ptr;
    if((ptr = strchr(str, '\n')) == NULL) {
        str[capacity - 1] = '\0';
    } else {
        *ptr = '\0';
    }
}


int main(void) {
    char userid[MAXLINE];
    char password[MAXLINE];

    /* Read a user id and password from stdin */
    printf("User id:\n");
    if((fgets(userid, MAXLINE, stdin)) == NULL) {
        fprintf(stderr, "Could not read from stdin\n"); 
        exit(1);
    }
    strip(userid, MAXPASSWD);

    printf("Password:\n");
    if((fgets(password, MAXLINE, stdin)) == NULL) {
        fprintf(stderr, "Could not read from stdin\n"); 
        exit(1);
    }
    strip(password, MAXPASSWD);

    int status;
    int fd[2];
    pipe(fd); 
    int r = fork();
    if (r > 0) {
        if (close(fd[0]) == -1) {
            perror("close");
            exit(1);
        }
        write(fd[1], userid, MAXPASSWD);
        write(fd[1], password, MAXPASSWD);
        if (close(fd[1]) == -1) {
            perror("close");
            exit(1);
        }
        wait(&status);
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 2) {
                printf("Invalid password\n");
            } else if (WEXITSTATUS(status) == 3) {
                printf("No such user\n");
            } else {
                printf("Password Verified\n");
            }
        }
    } else if (r == 0) {
        if (dup2(fd[0], fileno(stdin)) == -1) {
            perror("dup2");
            exit(1);
        }
        if (close(fd[0]) == -1) {
            perror("close");
            exit(1);
        }
        if (close(fd[1]) == -1) {
            perror("close");
            exit(1);
        }
        execlp("./validate", "validate", (char *)0);
        perror("validate");
        exit(EXIT_SUCCESS);
    } else { 
        perror("fork");
        exit(1);
    }
    return 0;
}
