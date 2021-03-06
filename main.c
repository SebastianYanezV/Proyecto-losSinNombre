#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include "treemap.h"
#include "heap.h"

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
    int quizesRealizados;
    int totalTarjetas;
    List *listaTarjetasUsuario; //Lista que almacena las tarjetas base de la aplicación y las tarjetas que agregue el usuario
    TreeMap *mapaAnverso; //Clave: anverso de la tarjeta ; Valor: tarjeta
    TreeMap *mapaReverso; //Clave: reverso de la tarjeta ; Valor: tarjeta
    Mheap *monticuloComplejidad; //Montículo para ordenar las tarjetas según la complejidad de cada una (de forma descendente)
}tipoUsuario;

typedef struct 
{
    TreeMap *mapaUsuarios; //Clave: nombre del usuario ; Valor: usuario
    TreeMap *mapaContrasenas; //Clave: contraseña ; Valor: contraseña
    List *listaTarjetas; //Lista que almacena las tarjetas base de la aplicación
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

//Función para crear la variable principal de la aplicación 
DatosAplicacion *crearDatos()
{
    DatosAplicacion *aux = (DatosAplicacion*) calloc (1, sizeof(DatosAplicacion));
    aux->mapaUsuarios = createTreeMap(lower_than_string);
    aux->mapaContrasenas = createTreeMap(lower_than_string);
    aux->listaTarjetas = createList();
    return aux;
}

//Función para crear la variable que almacenará todos los mapas
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

//función que lee el archivo principal (csv) de donde se obtienen las tarjetas
void leerDatosApp(DatosAplicacion *datosApp)
{
    FILE *archivoTarjetas = fopen("tarjetas.csv", "r");
    tipoTarjeta *tarjetaAux;
    char linea[1024];
    int largo, k;

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
                    for (k = 0; k < largo ; k++) tarjetaAux->reverso[k] = tolower(tarjetaAux->reverso[k]);
                    break;
                }
                case 1: //Se guarda la anverso
                {
                    tarjetaAux->anverso = reservarMemoria(largo);
                    strcpy(tarjetaAux->anverso, aux);
                    for (k = 0; k < largo ; k++) tarjetaAux->anverso[k] = tolower(tarjetaAux->anverso[k]);
                    break;
                }
                case 2: //Se guarda la oración
                {
                    tarjetaAux->oracion = reservarMemoria(largo); 
                    strcpy(tarjetaAux->oracion, aux);
                    break;
                }
            }
        }

        tarjetaAux->complejidad = 1;
        pushBack(datosApp->listaTarjetas, tarjetaAux);
    }
    
    fclose(archivoTarjetas);
}

//Función que crea una variable de tipo usuario una vez que el usuario se registre
void crearUsuario(DatosAplicacion *datosApp, char *nombre, char *contrasena)
{
    tipoUsuario *usuario = (tipoUsuario*) malloc(1 * sizeof(tipoUsuario));
    int largo;

    //Se guarda el nombre y la contraseña del usuario con los datos dados
    largo = strlen(nombre) + 1;
    usuario->nombre = (char*) malloc(largo * sizeof(char));
    strcpy(usuario->nombre, nombre);

    largo = strlen(contrasena) + 1;
    usuario->contrasena = (char*) malloc(largo * sizeof(char));
    strcpy(usuario->contrasena, contrasena);

    usuario->quizesRealizados = 0;
    usuario->totalTarjetas = 0;

    //se crean los distintos TDAs
    usuario->listaTarjetasUsuario = createList();
    usuario->monticuloComplejidad = createMheap();
    usuario->mapaAnverso = createTreeMap(lower_than_string);
    usuario->mapaReverso = createTreeMap(lower_than_string);

    tipoTarjeta *aux = firstList(datosApp->listaTarjetas);
    
    //Se recorre la lista de tarjetas de la app para poder ingresar sus tarjetas en los TDAs correspondientes del usuario
    while (aux != NULL)
    {
        usuario->totalTarjetas++;

        pushBack(usuario->listaTarjetasUsuario, aux);
        insertTreeMap(usuario->mapaAnverso, aux->anverso, aux);
        insertTreeMap(usuario->mapaReverso, aux->reverso, aux);
        heap_push(usuario->monticuloComplejidad, aux, aux->complejidad);
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

void mostrarTarjetas(tipoUsuario *usuario)
{
    int opcion2;

    do
    {
        printf("\n¿Cómo desea mostrar sus tarjetas?\n");
        printf("1.- Orden alfabético desde el reverso\n");
        printf("2.- Orden alfabético desde el anverso\n");
        printf("3.- Orden según complejidad\n");
        scanf("%d", &opcion2);
        getchar();
    } while (opcion2 < 0 || opcion2 > 4);
                
    //Una vez elegida la forma, se presentan las tarjetas según el orden deseado

    //Se recorre el mapa reverso para mostrar las tarjetas por orden alfabético desde el reverso
    if (opcion2 == 1)
    {
        Pair *aux = firstTreeMap(usuario->mapaReverso);
        tipoTarjeta *tarjeta;

        while (aux != NULL)
        {
            tarjeta = aux->value;

            printf("%s, ", tarjeta->reverso);
            printf("%s, ", tarjeta->anverso);
            printf("%s\n", tarjeta->oracion);

            aux = nextTreeMap(usuario->mapaReverso);
        }
    }

    //Se recorre el mapa anverso para mostrar las tarjetas por orden alfabético desde el anverso
    if (opcion2 == 2)
    {
        Pair *aux = firstTreeMap(usuario->mapaAnverso);
        tipoTarjeta *tarjeta;

        while (aux != NULL)
        {
            tarjeta = aux->value;
                        
            printf("%s, ", tarjeta->anverso);
            printf("%s, ", tarjeta->reverso);
            printf("%s\n", tarjeta->oracion);

            aux = nextTreeMap(usuario->mapaAnverso);
        }
    }

    //Se "recorre" el motículo para mostrar las tarjetas según la complejidad de cada una
    if (opcion2 == 3)
    {
        tipoTarjeta *tarjeta = heap_top(usuario->monticuloComplejidad);

        while (tarjeta != NULL)
        {
            printf("%s, ", tarjeta->anverso);
            printf("%s, ", tarjeta->reverso);
            printf("%s\n", tarjeta->oracion);

            heap_pop(usuario->monticuloComplejidad);
            tarjeta = heap_top(usuario->monticuloComplejidad);
        }

        tarjeta = firstList(usuario->listaTarjetasUsuario);

        while (tarjeta != NULL)
        {
            heap_push(usuario->monticuloComplejidad, tarjeta, tarjeta->complejidad);
            tarjeta = nextList(usuario->listaTarjetasUsuario);
        }
    }
}

void agregarTarjeta(tipoUsuario *usuario)
{
    tipoTarjeta *tarjetaAux;
    List *listaComplejidad;
    int largo, k;

    tarjetaAux = (tipoTarjeta*) calloc(1, sizeof(tipoTarjeta));

    char *anverso;
    char *reverso;
    char *frase;
                
    //Se le pide al usuario que ingrese los datos de su tarjeta nueva
    printf("Ingrese el anverso (palabra en español) de la tarjeta nueva:\n");
    leerChar(&anverso);
    largo = strlen(anverso) + 1;
    tarjetaAux->anverso = reservarMemoria(largo);
    strcpy(tarjetaAux->anverso, anverso);
    for (k = 0; k < largo ; k++) tarjetaAux->anverso[k] = tolower(tarjetaAux->anverso[k]);

    printf("Ingrese el reverso (palabra en inglés) de la tarjeta nueva:\n");
    leerChar(&reverso);
    largo = strlen(reverso) + 1;
    tarjetaAux->reverso = reservarMemoria(largo);
    strcpy(tarjetaAux->reverso, reverso);
    for (k = 0; k < largo ; k++) tarjetaAux->reverso[k] = tolower(tarjetaAux->reverso[k]);

    printf("Ingrese la frase que irá como referencia de la tarjeta nueva:\n");
    leerChar(&frase);
    largo = strlen(frase) + 1;
    tarjetaAux->oracion = reservarMemoria(largo);
    strcpy(tarjetaAux->oracion, frase);
                
    //Se verifica que la tarjeta nueva no exista
    if (searchTreeMap(usuario->mapaAnverso, tarjetaAux->anverso) != NULL)
    {
        printf("\nLa palabra ingresada ya existe por lo que no se pudo agregar.\n");
    }
    //Si la tarjeta no existe, se agrega a los distintos TDAs del usuario
    else
    {
        tarjetaAux->complejidad = 1;

        insertTreeMap(usuario->mapaAnverso, tarjetaAux->anverso, tarjetaAux);
        insertTreeMap(usuario->mapaReverso, tarjetaAux->reverso, tarjetaAux);
        heap_push(usuario->monticuloComplejidad, tarjetaAux, tarjetaAux->complejidad);
        pushBack(usuario->listaTarjetasUsuario, tarjetaAux);

        usuario->totalTarjetas++;

        printf("\nPalabra agregada correctamente.\n");
    }
}

void quizCompleto(tipoUsuario *usuario)
{
    if (usuario->quizesRealizados == 0)
    {
        printf("Esta es la primera vez que realiza un quiz, por lo que deberá reponder todas las tarjetas para medir sus conocimientos:\n");
    }

    if (usuario->quizesRealizados != 0 && usuario->quizesRealizados % 5 == 0)
    {
        printf("Ya ha pasado un tiempo desde que hizo un quiz con todas sus tarjetas, ¿no lo cree?\n");
        printf("Es momento de hacer uno para saber como va su aprendizaje!\n");
    }

    Pair *aux = firstTreeMap(usuario->mapaAnverso);
    tipoTarjeta *tarjeta;
    char *palabra;
    int largo, k, fallos, opcionPista;

    while (aux != NULL)
    {
        tarjeta = aux->value;
        fallos = 0;

        do
        {
            printf("%s, ", tarjeta->reverso);
            printf("Ingrese el significado de esta palabra en español: ");
            leerChar(&palabra);
            largo = strlen(palabra) + 1;
            for (k = 0; k < largo ; k++) palabra[k] = tolower(palabra[k]);
            printf("\n");

            if (strcmp(tarjeta->anverso, palabra) != 0) 
            {
                tarjeta->complejidad = tarjeta->complejidad * 1.5;
                fallos++;

                if (fallos >= 3)
                {
                    do
                    {
                        printf("¿Desea ver una pista?\n");
                        printf("1.- SI\n");
                        printf("2.- NO\n");
                        scanf("%d", &opcionPista);
                        getchar();
                    } while (opcionPista < 0 || opcionPista > 3);

                    if (opcionPista == 1) printf("%s\n", tarjeta->oracion);
                }
            }

            if (strcmp(tarjeta->anverso, palabra) == 0) tarjeta->complejidad = tarjeta->complejidad * 0.5;

        } while (strcmp(tarjeta->anverso, palabra) != 0);
                    
        aux = nextTreeMap(usuario->mapaAnverso);
    }

    tarjeta = heap_top(usuario->monticuloComplejidad);

    while (tarjeta != NULL)
    {
        heap_pop(usuario->monticuloComplejidad);
        tarjeta = heap_top(usuario->monticuloComplejidad);
    }

    tarjeta = firstList(usuario->listaTarjetasUsuario);

    while (tarjeta != NULL)
    {
        heap_push(usuario->monticuloComplejidad, tarjeta, tarjeta->complejidad);
        tarjeta = nextList(usuario->listaTarjetasUsuario);
    }

    usuario->quizesRealizados = usuario->quizesRealizados + 1;
}

void quizPersonalizado(tipoUsuario *usuario)
{
    tipoTarjeta *tarjeta;
    int cantidadTarjetas, fallos, largo, k, opcionPista;
    char *palabra;
    int cont = 0;

    do
    {
        printf("¿Cuántas tarjetas desea responder en este quiz?\n");
        scanf("%d", &cantidadTarjetas);
        getchar();
    } while (cantidadTarjetas <= 0 || cantidadTarjetas > usuario->totalTarjetas);

    while (cont < cantidadTarjetas)
    {
        tarjeta = heap_top(usuario->monticuloComplejidad);
        fallos = 0;

        do
        {
            printf("%s, ", tarjeta->reverso);
            printf("Ingrese el significado de esta palabra en español: ");
            leerChar(&palabra);
            largo = strlen(palabra) + 1;
            for (k = 0; k < largo ; k++) palabra[k] = tolower(palabra[k]);
            printf("\n");

            if (strcmp(tarjeta->anverso, palabra) != 0) 
            {
                tarjeta->complejidad = tarjeta->complejidad * 1.5;
                fallos++;

                if (fallos >= 3)
                {
                    do
                    {
                        printf("¿Desea ver una pista?\n");
                        printf("1.- SI\n");
                        printf("2.- NO\n");
                        scanf("%d", &opcionPista);
                        getchar();
                    } while (opcionPista < 0 || opcionPista > 3);

                    if (opcionPista == 1) printf("%s\n", tarjeta->oracion);
                }
            }

            if (strcmp(tarjeta->anverso, palabra) == 0) tarjeta->complejidad = tarjeta->complejidad * 0.5;

        } while (strcmp(tarjeta->anverso, palabra) != 0);

        heap_pop(usuario->monticuloComplejidad);
        cont++;
    }

    tarjeta = heap_top(usuario->monticuloComplejidad);

    while (tarjeta != NULL)
    {
        heap_pop(usuario->monticuloComplejidad);
        tarjeta = heap_top(usuario->monticuloComplejidad);
    }

    tarjeta = firstList(usuario->listaTarjetasUsuario);

    while (tarjeta != NULL)
    {
        heap_push(usuario->monticuloComplejidad, tarjeta, tarjeta->complejidad);
        tarjeta = nextList(usuario->listaTarjetasUsuario);
    }

    usuario->quizesRealizados = usuario->quizesRealizados + 1;
}

void menuDeQuizes(tipoUsuario *usuario)
{
    //Aquí se hace el quiz inicial y los quizes de control de aprendizaje
    if (usuario->quizesRealizados == 0 || usuario->quizesRealizados % 5 == 0)
    {
        quizCompleto(usuario);
    }
    //Aquí se hacen los quizes personalizados por el usuario
    else
    {
        quizPersonalizado(usuario);
    }
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
            //La opción 1 es la de ver las tarjetas del usuario
            case 1:
            {   
                mostrarTarjetas(usuario);
                break;
            }
            //La opción 2 es agregar una tarjeta al mazo de tarjetas que posee el usuario
            case 2:
            {
                agregarTarjeta(usuario);
                break;
            }
            //La opción 3 consta de realizar los distintos tipos de quizes
            case 3:
            {
                menuDeQuizes(usuario);
                break;
            }
            case 4:
            {
                printf("\nCerrando sesión. Se volverá al menú de inicio.\n");
                return;
            }
        }
    }
}

void registrarse(DatosAplicacion *datosApp, char *nombre, char *contrasena)
{
    //Se comprueba que el nombre ingresado no exista
    if (searchTreeMap(datosApp->mapaUsuarios, nombre) == NULL) 
    {
        //Se comprueba que la contraseña ingresada no exista
        if (searchTreeMap(datosApp->mapaContrasenas, contrasena) == NULL) 
        {
            //Si el nombre y la contraseña no existen entonces se puede crear el usuario
            crearUsuario(datosApp, nombre, contrasena);
        }
        else
        {
            //Si la contraseña ingresada ya existe se le pedirá al usuario que ingrese una nueva hasta que sea válida
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
        //Si el nombre ingresado ya existe se le pedirá al usuario que ingrese uno nuevo hasta que sea válido
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
        //Si el nombre ingresado es correcto se comprueba que la contraseña que ingrese sea correcta
        aux = searchTreeMap(datosApp->mapaUsuarios, nombre);
        usuario = aux->value;

        if (strcmp(usuario->contrasena, contrasena) == 0)
        {
            //Si lo es, se le permitirá el acceso 
            printf("Inicio de sesión válido.\n");
            menuPrincipal(datosApp, usuario);
        }
        else 
        {
            do
            {
                //Si no lo es, se preguntará hasta que ingrese la contraseña correcta
                printf("La contraseña ingresada es incorrecta, ingrese otra por favor.\n");
                leerChar(&contrasena);
            } while (strcmp(usuario->contrasena, contrasena) != 0);

            printf("Inicio de sesión válido.\n");
            menuPrincipal(datosApp, usuario);
        }
    }
    else
    {
        //Si no se ingresa el nombre correcto, se preguntará por el nombre hasta que este sea correcto
        do
        {
            printf("El nombre de usuario ingresado es incorrecto, ingrese otro por favor.\n");
            leerChar(&nombre);
        } while (searchTreeMap(datosApp->mapaUsuarios, nombre) == NULL);

        //Una vez esté correcto, se realizarán los mismos pasos ya hechos anteriormente
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
}

int main()
{
    DatosAplicacion *datosApp = crearDatos(); //Se crea la variable que almacena usuarios, contraseñas y tarjetas base de la app
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
            //Registro del usuario
            case 1:
            {
                char *nombre, *contrasena;
                printf("\nA continuación ingrese sus datos personales para registrarse en EN-Learning Cards:\n");
                printf("Nombre de usuario: ");
                leerChar(&nombre);
                printf("Contraseña: ");
                leerChar(&contrasena);
                printf("\n");

                registrarse(datosApp, nombre, contrasena);

                break;
            }
            //Inicio de sesión del usuario
            case 2:
            {
                char *nombre, *contrasena;
                printf("\nA continuación ingrese sus datos personales para iniciar sesión en EN-Learning Cards:\n");
                printf("Nombre de usuario: ");
                leerChar(&nombre);
                printf("Contraseña: ");
                leerChar(&contrasena);
                printf("\n");

                iniciarSesion(datosApp, nombre, contrasena);

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