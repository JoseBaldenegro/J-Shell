#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER 1024

typedef struct {
         char *nombre;
         off_t tamano;
} Archivos;

int comparar_archivos(const void *a, const void *b) {
        Archivos *archivoA = (Archivos *)a;
        Archivos *archivoB = (Archivos *)b;
        return (archivoB->tamano - archivoA->tamano);
}

int main(int argc, char *argv[]) {
        if (argc < 3) {
           printf("Uso: %s archivo_salida archivo1 archivo2 ...\n", argv[0]);
           return 1;
         }

        int total_archivos = argc - 2;
        Archivos *archivos = malloc(sizeof(Archivos) * total_archivos);


        for (int i = 0; i < total_archivos; i++) {
           int archivo = open(argv[i + 2], O_RDONLY);
           if (archivo < 0) {
              printf("Error! El archivo %s no existe\n",argv[i + 2]);
              free(archivos);
              return 1;
           }
           struct stat st;
           if (stat(argv[i + 2], &st) != 0) {
              perror(argv[i + 2]);
              free(archivos);
              return 1;
           }
        archivos[i].nombre = argv[i + 2];
        archivos[i].tamano = st.st_size;
       }

        int salida = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (salida < 0) {
           perror("Error al abrir el archivo de salida");
           return 1;
        }

        qsort(archivos, total_archivos, sizeof(Archivos), comparar_archivos);

        char buffer[BUFFER];

       for (int i = 0; i < total_archivos; i++) {
           int archivo = open(archivos[i].nombre, O_RDONLY);
           if (archivo < 0) {
              perror(archivos[i].nombre);
              free(archivos);
              close(salida);
              return 1;
          }

           ssize_t bytes;
           while ((bytes = read(archivo, buffer, BUFFER)) > 0) {
              write(salida, buffer, bytes);
           }

          close(archivo);
        }

       free(archivos);
       close(salida);
       printf("Archivos concatenados exitosamente\n");
       return 0;
}
