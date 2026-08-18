#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: divarch archivo_entrada lineas_por_archivo\n");
        return 1;
    }

    const char *nombreArchivo = argv[1];
    long lineas = strtol(argv[2], NULL, 10);
    if (lineas <= 0) {
        printf("Error: líneas por archivo debe ser positivo\n");
        return 1;
    }

    int entradafd = open(nombreArchivo, O_RDONLY);
    if (entradafd == -1) {
        printf("Error al abrir archivo de entrada\n");
        return 1;
    }

    char buffer[1024];
    int salidafd = -1;
    long lineascontadas = 0;
    int archivos = 0;
    ssize_t bytes_read;
    ssize_t pos = 0;

    while ((bytes_read = read(entradafd, buffer + pos, 1)) == 1) {
        if (buffer[pos]=='\n' || pos==1022) {
            buffer[pos + 1] ='\0';

            if (lineascontadas % lineas ==0) {
                if (salidafd != -1) close(salidafd);

                char archivoSalida[256];
                snprintf(archivoSalida, sizeof(archivoSalida), "%s_%02d.txt", nombreArchivo, ++archivos);
                salidafd = open(archivoSalida, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (salidafd == -1) {
                    printf("Error al crear archivo de salida\n");
                    close(entradafd);
                    return 1;
                }
            }

            write(salidafd, buffer, pos + 1);
            lineascontadas++;
            pos = 0;
        } else {
            pos++;
        }
    }

    if (pos > 0) {
        if (lineascontadas % lineas == 0) {
            if (salidafd != -1) close(salidafd);

            char archivoSalida[256];
            snprintf(archivoSalida, sizeof(archivoSalida), "%s_%02d.txt", nombreArchivo, ++archivos);
            salidafd = open(archivoSalida, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (salidafd == -1) {
                printf("Error al crear archivo de salida\n");
                close(entradafd);
                return 1;
            }
        }
        write(salidafd, buffer, pos);
        lineascontadas++;
    }

    if (salidafd != -1) close(salidafd);
    close(entradafd);
    if (lineas>lineascontadas){
        printf("Lineas pedidas mayor a lineas del archivo, solo se creo un archivo\n");
   } else {
         printf("Archivo dividido correctamente.\n");
  }
         printf("Nombres del archivo: %s_01,02,etc\n",nombreArchivo);
    return 0;
}
