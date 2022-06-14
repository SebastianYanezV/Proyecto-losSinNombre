#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "treemap.h"

typedef struct
{
    char *reverso;
    char *anverso;
    char *oracion;
    float complejidad;
}tipoTarjeta;

typedef struct
{
    char *nombre;
    char *contraseña;
    List *listaTarjetasUsuario;
    TreeMap *MapaComplejidadUsuario;
}tipoUsuario;

typedef struct 
{
    TreeMap *mapaUsuarios;
    TreeMap *mapaAnverso;
    TreeMap *mapaReverso;
    TreeMap *mapaComplejidad;
    List *listaTarjetas;
}DatosAplicacion;

//Función que lee el archivo csv
char* get_csv_field (char * tmp, int k) 
{
    int open_mark = 0;
    char* ret = (char*) malloc (100*sizeof(char));
    int ini_i = 0, i = 0;
    int j = 0;

    while(tmp[i+1] !='\0')
    {
        if(tmp[i]== '\"')
        {
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }
        if(open_mark || tmp[i]!= ';')
        {
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }
        if(tmp[i]== ';')
        {
            if(k==j) 
            {
               ret[i-ini_i ] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }
    if(k==j) 
    {
       ret[i-ini_i] = 0;
       return ret;
    }

    return NULL;
}

//Función para poder hacer los "createMap" de los TreeMap
int lower_than_string(void* key1, void* key2)
{
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

//Función para crear la variable que almacenará todos los mapas
DatosAplicacion *crearDatos()
{
    DatosAplicacion *aux = (DatosAplicacion*) calloc (1, sizeof(DatosAplicacion));
    aux->mapaUsuarios = createTreeMap(lower_than_string);
    aux->mapaAnverso = createTreeMap(lower_than_string);
    aux->mapaReverso = createTreeMap(lower_than_string);
    aux->mapaComplejidad = createTreeMap(lower_than_string);
    aux->listaTarjetas = createList();
    return aux;
}

void leerChar(char** charALeer)
{
    char aux[200];
    int largo;
    scanf("%[^\n]s", aux);
    getchar();
    largo = strlen(aux) + 1;
    (*charALeer) = (char*) calloc (largo, sizeof(char));
    strcpy((*charALeer), aux);
}

void mostrarPrimerMenu()
{
    printf("Bienvenido a EN-Learning Cards\n\n");
    printf("1.- Registrarse\n");
    printf("2.- Iniciar Sesión\n");
    printf("3.- Salir del programa\n");
}

int comprobarRegistro(DatosAplicacion *datosApp, char *nombre, char *contraseña)
{
    return 0;
}

void crearUsuario(DatosAplicacion *datosApp, char *nombre, char *contraseña)
{

}

int main()
{
    DatosAplicacion *datosApp = crearDatos();
    int opcion;

    while (1)
    {
        do
        {
            mostrarPrimerMenu();
            scanf("%d", &opcion);
            getchar();
        } while (opcion < 0 || opcion > 4);

        switch (opcion)
        {
            case 1:
            {
                char *nombre, *contraseña;
                printf("\nA continuación ingrese sus datos personales para registrarse en EN-Learning Cards:\n");
                printf("Nombre de usuario: ");
                leerChar(&nombre);
                printf("Contraseña: ");
                leerChar(&contraseña);
                printf("\n");

                if (comprobarRegistro(datosApp, nombre, contraseña) == 0) 
                {
                    printf("Registro de sesión correcto.\n");
                    crearUsuario(datosApp, nombre, contraseña);
                    break;
                }
                else
                {
                    do
                    {
                        printf("El nombre de usuario ingresado ya existe, ingrese otro por favor.\n");
                        leerChar(&nombre);
                    } while (comprobarRegistro(datosApp, nombre, contraseña) != 0);
                }

                printf("Registro de sesión correcto.\n");
                crearUsuario(datosApp, nombre, contraseña);
                break;
            }
            case 2:
            {
                char *nombre, *contraseña;
                printf("\nA continuación ingrese sus datos personales para iniciar sesión en EN-Learning Cards:\n");
                printf("Nombre de usuario: ");
                leerChar(&nombre);
                printf("Contraseña: ");
                leerChar(&contraseña);
                printf("\n");
                break;
            }
            case 3:
            {
                printf("\nGracias por utilizar EN-Learning Cards.\n");
                return 0;
            }
        }
    }
    return 0;
}
