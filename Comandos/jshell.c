#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define MAX_CMDS 8

int cantidadpipes(char *linea, char *cmds[]);
void cantidadargs(char *cmd, char *args[]);
int setDir(char **args);
void executar_comando(char *cmd, int prev_fd, int next_fd);
void mostrarHelp();
void shell();
int definepath();

int cantidadpipes(char *linea, char *cmds[])
{
    int count = 0;
    char *token = strtok(linea, "|");
    while (token != NULL && count < MAX_CMDS) {
        cmds[count++] = token;
        token = strtok(NULL, "|");
    }
    return count;
}

void cantidadargs(char *cmd, char *args[])
{
    int i = 0;
    args[i] = strtok(cmd, " ");
    while (args[i] != NULL && i < MAX_ARGS - 1) {
        args[++i] = strtok(NULL, " ");
    }
    args[i] = NULL;
}

int setDir(char **args)
{
    if (args[0] != NULL && !strcmp(args[0], "cd")) {
        char *dir = args[1];
        if (dir == NULL) {
            dir = getenv("HOME");
            if (dir == NULL) {
                printf("Error! HOME no definido\n");
                return 1;
            }
        }
        if (chdir(dir) != 0) {
            perror("cd");
        }
        return 1;
    }
    return 0;
}

void ejecutar_comando(char *cmd, int prev_fd, int next_fd)
{
    char *args[MAX_ARGS];

    cantidadargs(cmd, args);

    if (setDir(args)) return;

    pid_t pid = fork();
    if (pid == 0) {
        if (prev_fd != -1) {
            dup2(prev_fd, STDIN_FILENO);
            close(prev_fd);
        }
        if (next_fd != -1) {
            dup2(next_fd, STDOUT_FILENO);
            close(next_fd);
        }

        execvp(args[0], args);
        perror("Fallo execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

void mostrarHelp(){
        printf("Comandos propios del shell:\n");
        printf("unarch ---> Permite concatenar varios archivos a uno solo.\n");
        printf("divarch ---> Permite separar un archivo en una cantidad especificada de archivos.\n");
        printf("newline ---> Permite ingresar una nueva linea a cualquier archivo.\n");
        printf("memproc ---> Permite monitorear la cantidad de memoria utilizada por todos los procesos.\n");
        printf("cleanbin ---> Permite borrar archivos temporales que eststen almacenados por mucho tiempo sin ser modificados por mucho tiempo sin modificar dentro del directorio HOME.\n");
}

void shell()
{
    char linea[MAX_LINE];
    char *cmds[MAX_CMDS];
    int status;

    while (1) {
        printf("JSHELL> ");
        fflush(stdout);
        if (fgets(linea, MAX_LINE, stdin) == NULL) {
            break;
        }
        linea[strcspn(linea, "\n")] = '\0';
        if (!strlen(linea)) continue;
        else if (!strcmp(linea, "salir")) break;
        else if (!strcmp(linea, "help"))
        {
           mostrarHelp();
           continue;
        }



        int cant = cantidadpipes(linea, cmds);
        int prev_fd = -1;
        int pipefd[2];

        for (int i = 0; i < cant; i++) {
            if (i < cant - 1) {
                if (pipe(pipefd) == -1) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            } else {
                pipefd[1] = -1;
            }

            ejecutar_comando(cmds[i], prev_fd, pipefd[1]);

            if (prev_fd != -1) {
                close(prev_fd);
            }
            if (pipefd[1] != -1) {
                close(pipefd[1]);
            }

            prev_fd = pipefd[0];
        }
    }
}

int definepath()
{
    char path[1024];
    if (getcwd(path, sizeof(path)) == NULL) {
        perror("No se encontr       el directorio actual");
        return 0;
    }

    const char *pathactual = getenv("PATH");
    if (pathactual == NULL) {
        fprintf(stderr, "Error, no se encontr       el PATH actual\n");
        return 0;
    }

    size_t tamanopath = strlen(pathactual) + strlen(path) + 2;
    char *nuevopath = malloc(tamanopath);
    if (nuevopath == NULL) {
        perror("Fallo de memoria al asignar nuevo path");
        return 0;
    }

    strcpy(nuevopath, pathactual);
    strcat(nuevopath, ":");
    strcat(nuevopath, path);

    if (setenv("PATH", nuevopath, 1) != 0) {
        perror("Fallo al poner el nuevo PATH");
        free(nuevopath);
        return 0;
    }

    free(nuevopath);
    return 1;
}

int main()
{
    if (!definepath()) return 0;
    shell();
    return 0;
}
