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
    char *contrasena;
    List *listaTarjetasUsuario;
    TreeMap *MapaComplejidadUsuario;
}tipoUsuario;

typedef struct 
{
    TreeMap *mapaUsuarios;
    TreeMap *mapaAnverso;
    TreeMap *mapaReverso;
    TreeMap *mapaComplejidad;
    TreeMap *mapaContrasenas;
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
    aux->mapaContrasenas = createTreeMap(lower_than_string);
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

void crearUsuario(DatosAplicacion *datosApp, char *nombre, char *contrasena)
{
    tipoUsuario *usuario = (tipoUsuario*) malloc(1 * sizeof(tipoUsuario));
    int largo;

    largo = strlen(nombre) + 1;
    usuario->nombre = (char*) malloc(largo * sizeof(char));
    strcpy(usuario->nombre, nombre);

    largo = strlen(contrasena) + 1;
    usuario->contrasena = (char*) malloc(largo * sizeof(char));
    strcpy(usuario->contrasena, contrasena);

    usuario->listaTarjetasUsuario = createList();

    tipoTarjeta *aux = firstList(datosApp->listaTarjetas);

    while (aux != NULL)
    {
        pushBack(usuario->listaTarjetasUsuario, aux);
        aux = nextList(datosApp->listaTarjetas);
    }

    insertTreeMap(datosApp->mapaUsuarios, usuario->nombre, usuario);
    insertTreeMap(datosApp->mapaContrasenas, contrasena, contrasena);

    printf("Usuario creado correctamente.\n");
}

void mostrarSegundorMenu(tipoUsuario *usuario)
{
    printf("\nBienvenid@ %s\n\n", usuario->nombre);
    printf("¿Qué desea hacer en este momento?\n");
    printf("1.- Ver mis tarjetas de estudio\n");
    printf("2.- Agregar una nueva tarjeta a mi mazo\n");
    printf("3.- Empezar un quiz\n");
    printf("4.- Cerrar Sesión\n");
}

void menuPrincipal(DatosAplicacion *datosApp, tipoUsuario *usuario)
{
    int opcion;

    while (1)
    {
        do
        {
            mostrarSegundorMenu(usuario);
            scanf("%d", &opcion);
            getchar();
        } while (opcion < 0 || opcion > 5);

        switch (opcion)
        {
            case 1:
            {
                Pair *aux = firstTreeMap(datosApp->mapaComplejidad);
                List *lista = aux->value;
                tipoTarjeta *tarjeta = firstList(lista);

                while (aux != NULL)
                {
                    lista = aux->value;
                    tarjeta = firstList(lista);
                    
                    while (tarjeta != NULL)
                    {
                        printf("%s, ", tarjeta->anverso);
                        printf("%s, ", tarjeta->reverso);
                        printf("%s\n", tarjeta->oracion);
                        tarjeta = nextList(lista);
                    }
                    aux = nextTreeMap(datosApp->mapaComplejidad);
                }

                break;
            }
            case 2:
            {
                break;
            }
            case 3:
            {
                break;
            }
            case 4:
            {
                printf("\nCerrando sesión. Se volverá al menú de inicio.\n");
                return;
            }
        }
    }
<<<<<<< HEAD
=======
}

void registrarse(DatosAplicacion *datosApp, char *nombre, char *contrasena)
{
    if (searchTreeMap(datosApp->mapaUsuarios, nombre) == NULL) 
    {
        if (searchTreeMap(datosApp->mapaContrasenas, contrasena) == NULL) 
        {
            crearUsuario(datosApp, nombre, contrasena);
        }
        else
        {
            do
            {
                printf("La contraseña ingresada ya existe, ingrese otra por favor.\n");
                leerChar(&contrasena);
            } while (searchTreeMap(datosApp->mapaContrasenas, contrasena) != NULL);

            crearUsuario(datosApp, nombre, contrasena);
        }
    }
    else
    {
        do
        {
            printf("El nombre de usuario ingresado ya existe, ingrese otro por favor.\n");
            leerChar(&nombre);
        } while (searchTreeMap(datosApp->mapaUsuarios, nombre) != NULL);

        if (searchTreeMap(datosApp->mapaContrasenas, contrasena) == NULL) 
        {
            crearUsuario(datosApp, nombre, contrasena);
        }
        else
        {
            do
            {
                printf("La contraseña ingresada ya existe, ingrese otra por favor.\n");
                leerChar(&contrasena);
            } while (searchTreeMap(datosApp->mapaContrasenas, contrasena) != NULL);

            crearUsuario(datosApp, nombre, contrasena);
        }
    }
}

void iniciarSesion(DatosAplicacion *datosApp, char *nombre, char *contrasena)
{
    Pair *aux;
    tipoUsuario *usuario;

    if (searchTreeMap(datosApp->mapaUsuarios, nombre) != NULL)
    {
        aux = searchTreeMap(datosApp->mapaUsuarios, nombre);
        usuario = aux->value;

        if (strcmp(usuario->contrasena, contrasena) == 0)
        {
            printf("Inicio de sesión válido.\n");
            menuPrincipal(datosApp, usuario);
        }
        else 
        {
            do
            {
                printf("La contraseña ingresada es incorrecta, ingrese otra por favor.\n");
                leerChar(&contrasena);
            } while (strcmp(usuario->contrasena, contrasena) != 0);

            printf("Inicio de sesión válido.\n");
            menuPrincipal(datosApp, usuario);
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

        if (strcmp(usuario->contrasena, contrasena) == 0)
        {
            printf("Inicio de sesión válido.\n");
            menuPrincipal(datosApp, usuario);
        }
        else 
        {
            do
            {
                printf("La contraseña ingresada es incorrecta, ingrese otra por favor.\n");
                leerChar(&contrasena);
            } while (strcmp(usuario->contrasena, contrasena) != 0);

            printf("Inicio de sesión válido.\n");
            menuPrincipal(datosApp, usuario);
        }
    }
>>>>>>> 559df6b (Modularicé la función main del programa creando las funciones "registrarse" e "iniciarSesion". (Sebastián Yáñez))
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
                char *nombre, *contrasena;
                printf("\nA continuación ingrese sus datos personales para registrarse en EN-Learning Cards:\n");
                printf("Nombre de usuario: ");
                leerChar(&nombre);
                printf("Contrasena: ");
                leerChar(&contrasena);
                printf("\n");

<<<<<<< HEAD
                if (searchTreeMap(datosApp->mapaUsuarios, nombre) == NULL) 
                {
                    if (searchTreeMap(datosApp->mapaContrasenas, contrasena) == NULL) 
                    {
                        crearUsuario(datosApp, nombre, contrasena);
                    }
                    else
                    {
                        do
                        {
                            printf("La contrasena ingresada ya existe, ingrese otra por favor.\n");
                            leerChar(&contrasena);
                        } while (searchTreeMap(datosApp->mapaContrasenas, contrasena) != NULL);

                        crearUsuario(datosApp, nombre, contrasena);
                    }
                }
                else
                {
                    do
                    {
                        printf("El nombre de usuario ingresado ya existe, ingrese otro por favor.\n");
                        leerChar(&nombre);
                    } while (searchTreeMap(datosApp->mapaUsuarios, nombre) != NULL);

                    if (searchTreeMap(datosApp->mapaContrasenas, contrasena) == NULL) 
                    {
                        crearUsuario(datosApp, nombre, contrasena);
                    }
                    else
                    {
                        do
                        {
                            printf("La contraena ingresada ya existe, ingrese otra por favor.\n");
                            leerChar(&contrasena);
                        } while (searchTreeMap(datosApp->mapaContrasenas, contrasena) != NULL);

                        crearUsuario(datosApp, nombre, contrasena);
                    }
                }
=======
                registrarse(datosApp, nombre, contrasena);

>>>>>>> 559df6b (Modularicé la función main del programa creando las funciones "registrarse" e "iniciarSesion". (Sebastián Yáñez))
                break;
            }
            case 2:
            {
                char *nombre, *contrasena;
                printf("\nA continuación ingrese sus datos personales para iniciar sesión en EN-Learning Cards:\n");
                printf("Nombre de usuario: ");
                leerChar(&nombre);
                printf("Contrasena: ");
                leerChar(&contrasena);
                printf("\n");

<<<<<<< HEAD
                Pair *aux;
                tipoUsuario *usuario;

                if (searchTreeMap(datosApp->mapaUsuarios, nombre) != NULL)
                {
                    aux = searchTreeMap(datosApp->mapaUsuarios, nombre);
                    usuario = aux->value;

                    if (strcmp(usuario->contrasena, contrasena) == 0)
                    {
                        printf("Inicio de sesión válido.\n");
                        menuPrincipal(datosApp, usuario);
                    }
                    else 
                    {
                        do
                        {
                            printf("La contrasena ingresada es incorrecta, ingrese otra por favor.\n");
                            leerChar(&contrasena);
                        } while (strcmp(usuario->contrasena, contrasena) != 0);

                        printf("Inicio de sesión válido.\n");
                        menuPrincipal(datosApp, usuario);
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

                    if (strcmp(usuario->contrasena, contrasena) == 0)
                    {
                        printf("Inicio de sesión válido.\n");
                        menuPrincipal(datosApp, usuario);
                    }
                    else 
                    {
                        do
                        {
                            printf("La contrasena ingresada es incorrecta, ingrese otra por favor.\n");
                            leerChar(&contrasena);
                        } while (strcmp(usuario->contrasena, contrasena) != 0);

                        printf("Inicio de sesión válido.\n");
                        menuPrincipal(datosApp, usuario);
                    }
                }
=======
                iniciarSesion(datosApp, nombre, contrasena);
>>>>>>> 559df6b (Modularicé la función main del programa creando las funciones "registrarse" e "iniciarSesion". (Sebastián Yáñez))

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