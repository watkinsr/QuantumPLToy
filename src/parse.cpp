#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/parse.h"
#include "../include/QReg.h"

// Taken from: https://brennan.io/2015/01/16/write-a-shell-in-c/

int main(void)
{
    // Load config files, if any.

    // Run command loop.
    lsh_loop();

    // Perform any shutdown/cleanup.
    return 0;
}

void lsh_loop(void)
{
    char *line;
    char **args;
    int status;

    do
    {
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

char *lsh_read_line(void)
{
    char *line = NULL;
    size_t bufsize = 0; // have getline allocate a buffer for us

    if (getline(&line, &bufsize, stdin) == -1)
    {
        if (feof(stdin))
        {
            exit(EXIT_SUCCESS); // We recieved an EOF
        }
        else
        {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

char **lsh_split_line(char *line)
{
    size_t bufsize = LSH_TOK_BUFSIZE;
    int position = 0;
    char **tokens = (char **)malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);

    while (token != NULL)
    {
        // printf("token: %s\n", token);
        tokens[position++] = token;

        if (position >= bufsize)
        {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = (char **)realloc(tokens, bufsize * sizeof(char *));
            if (!tokens)
            {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

/*
  List of builtin commands, followed by their corresponding functions.
 */

int lsh_num_builtins()
{
    return sizeof(tokens) / sizeof(char *);
}

int lsh_init(char **args)
{
    printf("Found INIT token.\n");
    if (args[1] == NULL || args[2] == NULL || args[3] == NULL)
    {
        fprintf(stderr, "invalid args to INIT, example: INIT R2 2 0\n");
    }
    else
    {
        char *r = args[1];
        int arg1 = atoi(args[2]);
        int arg2 = atoi(args[3]);
        printf("Valid input, register: %s, arg1: %i, arg2: %i\n", r, arg1, arg2);

        QReg reg = QReg(arg1, arg2);
    }
    return 1;
}

/*
  Builtin function implementations.
*/

int lsh_help(char **args)
{
    int i;
    printf("Welcome to simple quantum interpreter (SQINT)");
    printf("Consult examples for example program text");

    return 1;
}

int lsh_exit(char **args)
{
    return 0;
}

int lsh_execute(char **args)
{
    int i;

    if (args[0] == NULL)
    {
        // An empty command was entered.
        return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++)
    {
        if (strcmp(args[0], tokens[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }
    return 0;
}
