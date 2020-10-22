#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	// INICIALITZACIONS
	// Abrimos el socket de escucha
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la maquina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 3
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
	int i;
	// Atenderemos solo 5 peticiones
	for(;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		//sock_conn es el socket que usaremos para este cliente
		
		//Bucle para consultar las peticiones del cliente
		int end = 0;
		while (end == 0)
		{		
			// Ahora recibimos su nombre, que dejamos en buff
			ret=read(sock_conn,peticion, sizeof(peticion));
			printf ("Recibido\n");
			
			// Tenemos que añadirle la marca de fin de string 
			// para que no escriba lo que hay despues en el buffer
			peticion[ret]='\0';
			
			//Escribimos el nombre en la consola
			
			printf ("Peticion: %s\n",peticion);
			
			char *p = strtok( peticion, "/");
			int codigo =  atoi (p);
			char nombre[20];
			if (codigo != 0)
			{
				p = strtok( NULL, "/");
				strcpy (nombre, p);
				printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
			}
			
			if (codigo == 0)
				end = 1;
			else if (codigo ==1){ //piden la longitud del nombre
				int length = strlen(nombre);
				sprintf (respuesta, "%d",length);
			}
			else if (codigo == 2){
				// quieren saber si el nombre es bonito
				if((nombre[0]=='M') || (nombre[0]=='S'))
					strcpy (respuesta,"SI");
				else
					strcpy (respuesta,"NO");
			}
			else if (codigo == 3) {
				p = strtok( NULL, "/");
				float altura = atof(p);
				if (altura > 1.75)
					sprintf(respuesta, "%s: eres alto", nombre);
				else
					sprintf(respuesta, "%s: eres bajo", nombre);
			}
			else if (codigo == 4){
				int i = 0;
				int j = (strlen(nombre) - 1);
				int palindrome = 1;
				for(int k = 0; k < strlen(nombre); k++)
				{
					nombre[k] = tolower(nombre[k]);
				}
				while ((palindrome == 1) && (i < strlen(nombre)))
				{
					if (nombre[i] == nombre[j]){
						i++;
						j--;
					}
					else
						palindrome = 0;
				}
				
				if (palindrome == 1)
					sprintf(respuesta, "%s: tu nombre es palindrome", nombre);
				else
					sprintf(respuesta, "%s: tu nombre no es palindrome", nombre);	
			}
			
			else{
				for(int k = 0; k < strlen(nombre); k++)
				{
					nombre[k] = toupper(nombre[k]);
				}
				
				sprintf(respuesta, "Tu nombre en mayusculas es: %s", nombre);
			}
			
			if (codigo != 0)
			{
				printf ("%s\n", respuesta);
				// Y lo enviamos
				write (sock_conn,respuesta, strlen(respuesta));
			}
		}
		// Se acabo el servicio para este cliente
		close(sock_conn); 
	}
}
