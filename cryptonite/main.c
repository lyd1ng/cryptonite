#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Wir definieren 4069 als unseren groeszten
//Input Buffer. Wir werden das brauchen
//um Dateien partiell einzulesen
#define MAX_BUFFER 4069

//Haendelt fatale Fehler, die 
//unausweichlich zur auszerplanmaezigen
//Terminierung des Programms fuehren
void fatal(char* error)
{
	printf("Fatal Error: %s\n", error);
	exit(-1);
}

//Liefert die groezse einer Datei
long int get_file_size(FILE* f)
{
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	rewind(f);
	return size;
}

//Verschluesselt die Datei in mit dem Inhalt der
//Datei key und speichert das Ergebnis in out
int crypt_file(FILE* in, FILE* out, FILE* key)
{
	long int in_size = get_file_size(in);
	long int key_size = get_file_size(key);

	if(in_size <= key_size)
	{
		char* in_buffer = (char*)malloc(MAX_BUFFER);
		char* key_buffer = (char*)malloc(MAX_BUFFER);
		while(!feof(in))
		{
			memset(in_buffer, 0, MAX_BUFFER );
			memset(key_buffer, 0, MAX_BUFFER);

			int in_counter = fread(in_buffer, 1, MAX_BUFFER, in);
			int key_counter = fread(key_buffer, 1, in_counter, key);
			if(key_counter < in_counter){return -1;}
			for(int i = 0; i < in_counter; i++){in_buffer[i] = in_buffer[i] ^ key_buffer[i];}
			fwrite(in_buffer, 1, in_counter, out);
		}
		free(in_buffer);
		free(key_buffer);
	}
	if(in_size > key_size)
	{

		char* in_buffer = (char*)malloc(MAX_BUFFER);
		char* key_buffer = (char*)malloc(MAX_BUFFER);
		while(!feof(in))
		{
			memset(in_buffer, 0, MAX_BUFFER);
			memset(key_buffer, 0, MAX_BUFFER);

			int in_counter = fread(in_buffer, 1, MAX_BUFFER, in);
			int key_counter = fread(key_buffer, 1, MAX_BUFFER, key);
			if(key_counter != in_counter)
			{
				rewind(key);
				fread(key_buffer + strlen(key_buffer), 1, in_counter - key_counter, key);
			}
			for(int i=0; i < in_counter; i++){in_buffer[i] = in_buffer[i] ^ key_buffer[i];}
			fwrite(in_buffer, 1, in_counter, out);
		}
		free(in_buffer);
		free(key_buffer);
	}
	return 0;
}

int main(int argc, char** argv)
{
	if(argc >= 2 && strcmp(argv[1], "-h") == 0){printf("usage <in, out, key>\n"); return 0;}
	if(argc < 3){printf("At least two parameters are needed!\n-h for help\n"); return 0;}

	FILE* in_file = fopen(argv[1], "r");
	if(in_file == NULL){fatal("Opening the input file!");}
	FILE* key_file = fopen(argv[3], "r");
	if(key_file == NULL){fatal("Opening the key file!");}
	FILE* out_file = fopen(argv[2], "a");
	if(out_file == NULL){fatal("Opening the output file!");}

	if(crypt_file(in_file, out_file, key_file) == -1){printf("Error reading the input buffers synchroniusly");}
	return 0;
}
