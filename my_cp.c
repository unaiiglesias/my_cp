#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        fprintf(stderr, "ERROR: Wrong amount of arguments");
        exit(1);
    }


    // Abrimos source y comprobamos que exista
    FILE *fsource = fopen(argv[1], "r");
    if (fsource == NULL)
    {
        perror("ERROR");
        exit(1);
    }


    struct stat statbuff;      // Struct que contendra todos los datos relativos a fsource, tipo inode
    lstat(argv[2], &statbuff); // Analizar statsbuff con lo pasado en destination (para luego ver si es dir o fichero)


    // Variables para leer y copiar el archivo de origen
    unsigned char buffer[100];
    size_t char_leidos;

    char ruta_dest_completa[1000];
    /* Contexto: los trings en C son una mierda [mierda = estaticos]
        Por como funciona strcat (si pasan ruta) necesitamos meter todo en un char* con espacio suficiente,
        pero como no sabemos como de larga sera la ruta que nos van a pasar, reservamos suficiente
        memoria como para que quepa (casi) lo que sea, 1000 bytes van bien. 
        Ademas, para no repetir codigo, usaremos esta misma variable para el bucle de lectura.
    */


    if (S_ISDIR(statbuff.st_mode))
    {
        /*
            Destination es un directorio (o una ruta a directorio) donde tenemos que copiar source
            Source puede ser el nombre de un fichero o una ruta a un fichero
        */

        // Como source puede ser una ruta, la tokenizamos para quedarnos solo con el nombre del fichero
        // (El ultimo fragmento tras '/')
        // Que source sea una ruta no nos molesta para leer, pero si dest es un directorio, necesitamos el nombre
        // del archivo source, ya que sera el nombre del archivo dest
        char *token;
        char *nombre;
        char separador[] = "/";
        char *savestr = NULL;
        token = strtok_r(argv[1], separador, &savestr);
        nombre = token;

        while (token != NULL)
        {
            nombre = token;
            token = strtok_r(NULL, separador, &savestr);
        }
        
        /* 

            Nos han pasado en argv[1] el nombre (o ruta) del fichero a copiar, pero tenemos el nombre del archivo a crear
            en la variable nombre.

            Ahora, tenemos que unir destino (que es una ruta) con nombre.
            Asi, obtnedremos algo del estilo: directorio/fichero.txt

        */
    
        // concatenamos "argv[2]/nombre" y lo metemos en ruta_dest_completa
        strcpy(ruta_dest_completa, strcat(strcat(argv[2], "/"), nombre));
        // (esperemos que no pasen una ruta de mas de 1000 caracteres)

    }
    else // S_ISREG(statbuff.st:mode)
    {
        /*
            Destination es el nombre del (o ruta al) fichero al que copiar
        */

        // queremos escribir en el nombre del archivo que nos han pasado
        strcpy(ruta_dest_completa, argv[2] );
    }


    FILE *fdestination = fopen(ruta_dest_completa, "w");
    if (fdestination == NULL) 
    {
        // Nos han pasado como destino un directorio que no existe
        perror("ERROR");
        exit(1);
    }

    // Bucle de lectura y copiado de source en destination
    do
    {
        char_leidos = fread(buffer, sizeof(char), sizeof(buffer), fsource); // leer en source
        fwrite(buffer, sizeof(char), char_leidos, fdestination);            // Escribir en destination
    } while (char_leidos != 0);                                             // Mientras se lea algun elemento (Char) escribimos en el fichero

    return 0; // exito
}
