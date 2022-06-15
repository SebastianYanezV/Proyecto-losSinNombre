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
    printf("\nBienvenido a EN-Learning Cards\n\n");
    printf("1.- Registrarse\n");
    printf("2.- Iniciar Sesión\n");
    printf("3.- Salir del programa\n");
}

char* reservarMemoria(int largo)
{
    char *aux = (char*) calloc(largo, sizeof(char));
    return aux;
}

void leerDatosApp(DatosAplicacion *datosApp)
{
    FILE *archivoTarjetas = fopen("tarjetas.csv", "r");
    tipoTarjeta *tarjetaAux;
    char linea[1024];
    int largo;

    while (fgets (linea, 1023, archivoTarjetas) != NULL) 
    {
        tarjetaAux = (tipoTarjeta*) calloc(1, sizeof(tipoTarjeta));
        for (int i = 0 ; i < 3 ; i = i + 1) //Se itera 3 veces porque cada tarjeta tiene 3 características
        {
            char *aux = get_csv_field(linea, i);
            largo = strlen(aux) + 1;

            switch (i)
            {
                case 0: //Se guarda el reverso
                {
                    tarjetaAux->reverso = reservarMemoria(largo);
                    strcpy(tarjetaAux->reverso, aux);
                    break;
                }
                case 1: //Se guarda la anverso
                {
                    tarjetaAux->anverso = reservarMemoria(largo);
                    strcpy(tarjetaAux->anverso, aux);
                    break;
                }
                case 2: //Se guarda la oracion
                {
                    tarjetaAux->oracion = reservarMemoria(largo); 
                    strcpy(tarjetaAux->oracion, aux);
                    break;
                }
            }
        }

        tarjetaAux->complejidad = 1;
        pushBack(datosApp->listaTarjetas, tarjetaAux);
        insertTreeMap(datosApp->mapaAnverso, tarjetaAux->anverso, tarjetaAux);
        insertTreeMap(datosApp->mapaReverso, tarjetaAux->reverso, tarjetaAux);
    }
    
    insertTreeMap(datosApp->mapaComplejidad, &tarjetaAux->complejidad, datosApp->listaTarjetas);
    fclose(archivoTarjetas);
}

void crearUsuario(DatosAplicacion *datosApp, char *nombre, char *contraseña)
{
    tipoUsuario *usuario = (tipoUsuario*) malloc(1 * sizeof(tipoUsuario));
    int largo;

    largo = strlen(nombre) + 1;
    usuario->nombre = (char*) malloc(largo * sizeof(char));
    strcpy(usuario->nombre, nombre);

    largo = strlen(contraseña) + 1;
    usuario->contraseña = (char*) malloc(largo * sizeof(char));
    strcpy(usuario->contraseña, contraseña);

    usuario->listaTarjetasUsuario = createList();

    tipoTarjeta *aux = firstList(datosApp->listaTarjetas);

    while (aux != NULL)
    {
        pushBack(usuario->listaTarjetasUsuario, aux);
        aux = nextList(datosApp->listaTarjetas);
    }

    insertTreeMap(datosApp->mapaUsuarios, usuario->nombre, usuario);

    printf("Usuario creado correctamente.\n");
}

void menuPrincipal()
{

}

int main()
{
    DatosAplicacion *datosApp = crearDatos();
    leerDatosApp(datosApp);
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

                if (searchTreeMap(datosApp->mapaUsuarios, nombre) == NULL) 
                {
                    crearUsuario(datosApp, nombre, contraseña);
                }
                else
                {
                    do
                    {
                        printf("El nombre de usuario ingresado ya existe, ingrese otro por favor.\n");
                        leerChar(&nombre);
                    } while (searchTreeMap(datosApp->mapaUsuarios, nombre) != NULL);

                    crearUsuario(datosApp, nombre, contraseña);
                }
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

                Pair *aux;
                tipoUsuario *usuario;

                if (searchTreeMap(datosApp->mapaUsuarios, nombre) != NULL)
                {
                    aux = searchTreeMap(datosApp->mapaUsuarios, nombre);
                    usuario = aux->value;

                    if (strcmp(usuario->contraseña, contraseña) == 0)
                    {
                        printf("Inicio de sesión válido.\n");
                        menuPrincipal();
                    }
                    else 
                    {
                        do
                        {
                            printf("La contraseña ingresada es incorrecta, ingrese otra por favor.\n");
                            leerChar(&contraseña);
                        } while (strcmp(usuario->contraseña, contraseña) != 0);

                        printf("Inicio de sesión válido.\n");
                        menuPrincipal();
                    }
                }
                else
                {
                    do
                    {
                        printf("El nombre de usuario ingresado es incorrecto, ingrese otro por favor.\n");
                        leerChar(&nombre);
                    } while (searchTreeMap(datosApp->mapaUsuarios, nombre) == NULL);

                    aux = searchTreeMap(datosApp->mapaUsuarios, nombre);
                    usuario = aux->value;

                    if (strcmp(usuario->contraseña, contraseña) == 0)
                    {
                        printf("Inicio de sesión válido.\n");
                        menuPrincipal();
                    }
                    else 
                    {
                        do
                        {
                            printf("La contraseña ingresada es incorrecta, ingrese otra por favor.\n");
                            leerChar(&contraseña);
                        } while (strcmp(usuario->contraseña, contraseña) != 0);

                        printf("Inicio de sesión válido.\n");
                        menuPrincipal();
                    }
                }

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
