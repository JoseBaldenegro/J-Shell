#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int contar_lineas(const char *archivo)
{
    FILE *f = fopen(archivo, "r");
    if (!f) return -1;

    int ch, lineas = 0;
    while ((ch = fgetc(f)) != EOF) {
        if (ch == '\n') lineas++;
    }
    fclose(f);
    return lineas;
}

int main (int argc, char *argv[]){
        if (argc != 4){
           fprintf (stderr, "ARGUMENTOS INSUFICIENTES\n");
           return 1;
        }

        char *archivo = argv[1];
        int linea_num = atoi(argv[2]);
        char *texto = argv[3];

        if (linea_num <= 0){
           fprintf(stderr, "NUMERO DE LINEA INVALIDO\n");
           return 1;
        }

    //Contar lineas del archivo
    int total_lineas = contar_lineas(archivo);
    if (linea_num > total_lineas + 1) {
        fprintf(stderr,
                "La linea %d excede el numero de lineas (%d) del archivo. "
                "El texto sera insertado al final.\n",
                linea_num, total_lineas);
        linea_num = total_lineas + 1;
    }


    //Nombre para el archivo temporal
        char temporal[256];
        pid_t pid = getpid();
        snprintf(temporal, sizeof(temporal), "insercion%d.tmp", pid);

    //Crear el archivo temporal
        int temp_fd = open(temporal, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (temp_fd == -1){
           perror("insertar");
           return 1;
        }

    fprintf(stderr, "Insertando nueva línea en el archivo '%s'...\n", archivo);

        //Redirigir salida estándar al archivo temporal
        if (dup2(temp_fd, STDOUT_FILENO) == -1){
           perror("dup2");
           close(temp_fd);
           unlink(temporal);
           return 1;
        }

    //Crear tubería
        int pipefd[2];
        if (pipe(pipefd) == -1){
           perror("pipe");
           return 1;
        }

    //Crear proceso hijo
        pid_t pid_fork = fork();
        if (pid_fork == -1){
           perror("fork");
           return 1;
        }

        //Proceso hijo
        if (pid_fork == 0){
           close(pipefd[0]);
           dup2(pipefd[1], STDOUT_FILENO); //Redirigir stdout a la escritura del pipe
           execlp("cat", "cat", archivo, NULL);
           perror("execlp");
           exit(1);
        }

        //Proceso padre
        close(pipefd[1]);
        FILE *tuberia = fdopen(pipefd[0], "r");

        char buffer[1024];
        int linea_actual = 1;
        int insertado = 0;

        while (fgets(buffer, sizeof(buffer), tuberia)){
           if (linea_actual == linea_num){
              printf("%s\n", texto);
              insertado = 1;
           }
           fputs(buffer, stdout);
           linea_actual++;
        }

        // Si el archivo está vacío o queremos insertar al final exacto
    if (!insertado) {
        printf("%s\n", texto);
    }

        fclose(tuberia);
        close(temp_fd);
        wait(NULL);     //Esperar al hijo

        //Reemplazar archivo original
        if (rename(temporal, archivo)){
           perror("insertar");
           unlink(temporal);
           return 1;
        }

    fprintf(stderr, "Linea insertada correctamente en el archivo '%s'.\n", archivo);
        return 0;
}
