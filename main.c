#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 4069
char* USAGE = (char*)"usage: <in, out, key>\n";
char* TO_FEW_ARGS = (char*)"Three parameters are needed!\nusage <in, out, key>\nUser -h/-help for further information\n";
char* HELP = (char*)"The cryptonite programme encrypts data with a given key file.\nThe algorithm is a simple XOR encryption.\nIf the key file is smaller than the data file the key file will be rewinded,\nso parts of the key file might be uses multiple times.!\nTo enhance the strength of the encryption make sure that the key file is equal or greater in lenght.\n";
                      

// Handle fatal errors by
// terminating the programme
void fatal(char* error)
{
	printf("Fatal Error: %s\n", error);
	exit(-1);
}

// Get the size of a file
long int get_file_size(FILE* f)
{
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	rewind(f);
	return size;
}

// En-/Decrypts the file with the content of the key file
// and safes the output in out
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
		// It is not allowed to use keys smaller than the file to encrypt
		return -2;
	}
	return 0;
}

int main(int argc, char** argv)
{
	if(argc >= 2 && strcmp(argv[1], "-h") == 0){printf("%s", HELP); printf("%s", USAGE); return 0;}
	if(argc < 3){printf("%s", TO_FEW_ARGS); return 0;}

	if(strcmp(argv[1], argv[2]) == 0)
	{
		fatal("In- and outputfile has to be different!");
	}

	FILE* in_file = fopen(argv[1], "r");
	if(in_file == NULL){fatal("Opening the input file!");}
	FILE* key_file = fopen(argv[3], "r");
	if(key_file == NULL){fatal("Opening the key file!");}
	FILE* out_file = fopen(argv[2], "w");
	if(out_file == NULL){fatal("Opening the output file!");}

	int result = crypt_file(in_file, out_file, key_file);
	if( result == -1 ) { fatal("Error reading the input buffers synchronously"); }
	if( result == -2 ) { fatal("The key file has to be longer than the file to encrypt"); }
	return 0;
}
