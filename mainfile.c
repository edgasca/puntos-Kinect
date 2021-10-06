/*Author: Eduardo de Jesús Gasca Laguna.*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//tiempo: t=1/30s
typedef struct Nodo
{
    float coordenadas[25][3];
    struct Nodo *siguiente;
}nodo;

typedef struct Actividad
{
    float velocidad;
    nodo *posicion;
    struct Actividad *siguiente;

}actividad;

typedef struct Persona
{
    int idPersona;
    actividad *a;
    struct Persona *siguiente;
}persona;

void *mostrarDatos(persona *listaDePersonas);
void mostrar(float **matriz,int filas,int columnas);

float velocidadJoin(float posicionInicial, float posicionFinal, int nMuestreos); //Velocidad por articulacion
float velocidadPromedioPorEje(float *eje); //Velocidad de cada eje
float velocidadResultante(float Vx, float Vy, float Vz);

persona *obtenerDatos(char *file, persona *listaDePersonas, int nPersona, int orden);

nodo *agregarNuevonodo(nodo *listaDeNodos, float **datos, int tn);
actividad *agregarNuevaActividad(actividad *listaDeActividades, int tn, float **datosActividad);
persona *agregarNuevaPersona(persona *listaDePersonas,int tiempo_A1, int tiempo_A2, int nPersona, float **datosActividad1, float **datosActividad2, int orden);

int main()
{
    int numeroDePersonas;
    int orden;
    char nombreDeArchivo[12]="puntos0.txt ";

    persona *listaDePersonas=NULL;

    system("cls");
    printf("*****PROYECTO FINAL.*****\n");
    printf("Ingrese el numero de personas:");
    scanf("%i", &numeroDePersonas);

    printf("Como desea ordenar la lista?\n");
    printf("1. Por su velocidad en la actividad 1.\n");
    printf("2. Por su velocidad en la actividad 2.\n");
    scanf("%d", &orden);

    system("cls");
    for (int i=1; i <=numeroDePersonas; i++)
    {
        sprintf(nombreDeArchivo,"puntos%i.txt",i);
        listaDePersonas = obtenerDatos(nombreDeArchivo, listaDePersonas,i, orden);
    }

    mostrarDatos(listaDePersonas);

    printf("La lista en orden por su velocidad en la \"actividad %d\"\nse encuentra en \"Datos.txt\" \n\n", orden);

    free(listaDePersonas); //LiberarMemoria

    getch();
    
    return 0;
}

persona *obtenerDatos(char *file, persona *listaDePersonas, int nPersona, int orden)
{
    FILE *archivo=NULL;
    float **DataA1=NULL;
    float **DataA2=NULL;

    int time_A1, time_A2, filas;

    archivo = fopen(file,"rt");
    
    if(archivo == NULL)
    {
        printf("\n***No se pudo encontrar \"%s\" \n", file);
        printf("Verifique el numero de personas o que el archivo se encuentra en su sistema...\n\n");
        getch();
        exit(-1);
    }

    
    //Obteniendotiempos y generando memoria
    //ACTIVIDAD1
    fscanf(archivo,"%d", &time_A1);
    filas = 25*time_A1;

    DataA1 = (float **)malloc(sizeof(float *)*filas);

        for(int i=0; i<filas;i++)
        {
            DataA1[i] = (float *)malloc(sizeof(float)*3);
        }
    
    for(int i=0; i<filas; i++)
        fscanf(archivo,"%f %f %f", &DataA1[i][0], &DataA1[i][1], &DataA1[i][2]);
    
    //ACTIVIDAD2
    fscanf(archivo,"%d", &time_A2);
    filas = 25*time_A2;
    
    DataA2=(float **)malloc(sizeof(float *)*filas);

        for(int i=0; i<filas;i++)
        {
            DataA2[i] = (float *)malloc(sizeof(float)*3);
        }
    
    for(int i=0; i<filas; i++)
        fscanf(archivo,"%f %f %f", &DataA2[i][0], &DataA2[i][1], &DataA2[i][2]);


    listaDePersonas = agregarNuevaPersona(listaDePersonas,time_A1, time_A2, nPersona, DataA1, DataA2, orden);
    
    fclose(archivo);
    free(DataA1);
    free(DataA2);
    printf("Datos obtenidos correctamente...\n\n");

    return listaDePersonas;

}
persona *agregarNuevaPersona(persona *listaDePersonas,int tiempo_A1, int tiempo_A2, int nPersona, float **datosActividad1, float **datosActividad2, int orden)
{
    persona *nuevaPersona;
    persona *auxiliar1 = listaDePersonas;
    persona *auxiliar2;

    nuevaPersona = malloc(sizeof(persona));

    nuevaPersona->idPersona = nPersona;

    printf("\n\nDatos de persona (%d)\n\n", nuevaPersona->idPersona);
    //2 ActividadesporPersona

    nuevaPersona->a = agregarNuevaActividad((nuevaPersona->a),tiempo_A1, datosActividad1);
    nuevaPersona->a = agregarNuevaActividad((nuevaPersona->a),tiempo_A2, datosActividad2);  

    nuevaPersona->siguiente=NULL;

    //OrdenadosPorVelocidad de actividad 1
    if(orden==1)
    {
        while (( auxiliar1 != NULL) && ((auxiliar1->a->velocidad) < (nuevaPersona->a->velocidad)))
        {
            auxiliar2 = auxiliar1;
            auxiliar1 = auxiliar2->siguiente;
        }
    }
    
    //OrdenadosPorVelocidad de actividad 2
    if(orden==2)
    {
        while((auxiliar1 != NULL) && ((auxiliar1->a->siguiente->velocidad) < (nuevaPersona->a->siguiente->velocidad)))
        {
            auxiliar2 = auxiliar1;
            auxiliar1 = auxiliar2->siguiente;
        }
    }
    
    if (listaDePersonas == auxiliar1)
    {
        listaDePersonas =  nuevaPersona;
    }
    else
    {
        auxiliar2->siguiente = nuevaPersona;
    }
    
    nuevaPersona->siguiente = auxiliar1;
    
    printf("Persona agregada correctamente...\n");

    return listaDePersonas;
}

actividad *agregarNuevaActividad(actividad *listaDeActividades, int tn, float **datosActividad)
{
    actividad *nuevaActividad, *auxiliar;
    int salto=0;
    float Vx[25], Vy[25], Vz[25], Vpx, Vpy, Vpz, velocidadActividad;
    float xInicial[25], xFinal[25], yInicial[25], yFinal[25], zInicial[25], zFinal[25];

    nuevaActividad = malloc(sizeof(actividad));

    //Numero de nodos igual a tn
    for(int i=0; i<tn; i++)
        nuevaActividad->posicion = agregarNuevonodo(nuevaActividad->posicion,datosActividad, i);

    //Calculo de velocidades promedio para cada articulacion por eje
    while ((nuevaActividad->posicion) != NULL)
            {
                for(int i=0;i<25;i++)
                {

                        if(salto==0)
                        {
                            xInicial[i]= nuevaActividad->posicion->coordenadas[i][0];
                            yInicial[i]= nuevaActividad->posicion->coordenadas[i][1];
                            zInicial[i]= nuevaActividad->posicion->coordenadas[i][2];
                        }
                        
                        if (salto==(tn-1))
                        {
                            xFinal[i]= nuevaActividad->posicion->coordenadas[i][0];
                            yFinal[i]= nuevaActividad->posicion->coordenadas[i][1];
                            zFinal[i]= nuevaActividad->posicion->coordenadas[i][2];
                            
                            //Velocidad promedio de cada articulacion por eje
                            Vx[i] = velocidadJoin(xInicial[i],xFinal[i], tn);
                            Vy[i] = velocidadJoin(yInicial[i],yFinal[i], tn);
                            Vz[i] = velocidadJoin(zInicial[i],zFinal[i], tn);
                        }
                }
                    
                salto++;
                nuevaActividad->posicion = (nuevaActividad->posicion)->siguiente;
            }

    Vpx = velocidadPromedioPorEje(Vx);
    Vpy = velocidadPromedioPorEje(Vy);
    Vpz = velocidadPromedioPorEje(Vz);

    velocidadActividad = velocidadResultante(Vpx,Vpy,Vpz);
    printf("La velocidad promedio de la actividad es: %f\n", velocidadActividad);

    nuevaActividad->velocidad = velocidadActividad;

    if(listaDeActividades==NULL)
    {
        listaDeActividades = nuevaActividad;
    }
    else
    {
        auxiliar = listaDeActividades;
        while (auxiliar->siguiente != NULL)
        {
            auxiliar = auxiliar->siguiente;
        }
        auxiliar->siguiente = nuevaActividad;
    }

    printf("Actividad agregada con exito...\n\n");

    return listaDeActividades;
}

nodo *agregarNuevonodo(nodo *listaDeNodos, float **datos, int tn)
{
    nodo *nuevoNodo, *auxiliar;

    nuevoNodo = malloc(sizeof(nodo));

    for(int join=0; join<25; join++)
    {
        nuevoNodo->coordenadas[join][0] = datos[join+tn*25][0];
        nuevoNodo->coordenadas[join][1] = datos[join+tn*25][1];
        nuevoNodo->coordenadas[join][2] = datos[join+tn*25][2];
    }

    if(listaDeNodos==NULL)
    {
        listaDeNodos = nuevoNodo;
    }
    else
    {
        auxiliar = listaDeNodos;
        while (auxiliar->siguiente != NULL)
        {
            auxiliar = auxiliar->siguiente;
        }
        auxiliar->siguiente = nuevoNodo;
    }

    return listaDeNodos;
}

float velocidadJoin(float posicionInicial, float posicionFinal, int nMuestreos)
{
    float tiempo, velocidadJ;
    tiempo = nMuestreos*(1.0/30.0);

    velocidadJ = (posicionFinal-posicionInicial)/tiempo;

    return velocidadJ;
}

float velocidadPromedioPorEje(float *eje)
{
    float velocidadPromedio=0;

    for(int i=0; i<25; i++)
        velocidadPromedio += eje[i];

    return (velocidadPromedio/25.0); 
}

float velocidadResultante(float Vx, float Vy, float Vz)
{
    float V;
    
    V = sqrt(pow(Vx,2) + pow(Vy,2) + pow(Vz,2));

    return V;
}

void *mostrarDatos(persona *listaDePersonas)
{
    FILE *archivo=NULL;

    archivo = fopen("Datos.txt","wt");

    while (listaDePersonas != NULL)
    {
        fprintf(archivo,"**Persona (%d)\n", listaDePersonas->idPersona);
        printf("**Persona (%d)\n", listaDePersonas->idPersona);
        
        while ((listaDePersonas->a) != NULL)
        {

            fprintf(archivo,"Velocidad = %f\n", (listaDePersonas->a)->velocidad); 
            printf("Velocidad = %f\n", (listaDePersonas->a)->velocidad);

            listaDePersonas->a = (listaDePersonas->a)->siguiente;
        }
        
        listaDePersonas=listaDePersonas->siguiente;
    }

    fclose(archivo);
}
