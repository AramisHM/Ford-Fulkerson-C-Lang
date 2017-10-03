/*  Ford-Fulkerson in C-Lang
	Released under the MIT License
*/

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Returns the size of a given file*/
long ahmlu_get_file_size(char* file_path) {
	FILE *fp;
	long file_size;

	fp = fopen (file_path , "rb");

	if(!fp) {
		printf("Can't find or open the file \"%s\" for reading.\n", file_path);
		return 0;
	}

	// Get the file size in bytes.
	fseek(fp, 0L, SEEK_END);
	file_size = ftell(fp);
	rewind(fp);

	fclose (fp);

	return file_size;
}

/*Returns a char array formated data from a file
WARNING !not null terminated!!!
DO NOT FORGET TO FREE THE RETURN OF THIS FUNCTION!!!
returns 0 if fails*/
unsigned char *ahmlu_create_char_array_from_file(char* file_path) {
	unsigned char *ret; /* we will store the file data here */
	long fsize; /* the file size */
	FILE * pFile; /* file pointer */

	fsize = ahmlu_get_file_size(file_path);
	ret = (unsigned char*)malloc(fsize*sizeof(unsigned char)); /* allocate needed memory */

	pFile = fopen (file_path, "rb");
  	if (!pFile) {
		printf("Can't find or open the file \"%s\" for reading.\n", file_path);
		fputs ("File error",stderr);
		return 0;
	}
	fread (ret,1,fsize,pFile);
	fclose(pFile);

	return ret;
}

/*	loads all cities from the given file to parse into the cities array
	returns the number of cities loaded */
int get_n_cities(char* file_to_parse) {
	char *raw_file;
	long fs = ahmlu_get_file_size(file_to_parse);
	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	/* get cities names */
	char *cities = strtok(raw_file,"\n"); /*first line*/
	char *city_name = strtok(cities,"\",");
	
	int city_count = 1;

	while (city_name) {
		city_name = strtok(NULL,"\",");
		if (city_name &&
			city_name[0] != ',' &&
			city_name[0] != '\r')
			++city_count;
	}
	free(raw_file);
	return city_count;
}

/* parse file and get the city names*/
char **get_city_names(char *file_to_parse, int city_count) {
	char *raw_file;
	char **city_names = (char**)malloc(sizeof(char*)*city_count);
	
	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	int aux= 0;
	char *cities = strtok(raw_file,"\n"); /*first line*/
	char *city_name = strtok(cities,"\",");
	while(city_name) {
		if (city_name &&
			city_name[0] != ',' &&
			city_name[0] != '\r') {
			city_names[aux] = (char*)malloc(sizeof(char)*(strlen(city_name)+1));
			memccpy(city_names[aux], city_name, 1, strlen(city_name));
			city_names[aux][strlen(city_name)]=0;
			++aux;
		}
		city_name = strtok(NULL,"\",");
	}

	free(raw_file);
	/* debug: print city names */
	for(int i =0; i < city_count; ++i)
		printf("%s\n",city_names[i]);
	
	return city_names;
}

int *get_distances_vecotr(char* file_to_parse, int city_count) {
	char *raw_file;
	
	int *distances = NULL;

	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	distances = (int*)malloc(sizeof(int)*(city_count*city_count));

	strtok(raw_file,"\n\r"); /*ignore first line*/
	char *distance = strtok(NULL,"\n\r, ");
	int aux = 0;
	while (distance) {
		distances[aux++] = atoi(distance);
		distance = strtok(NULL,"\n\r, ");
		//printf("%d ",distances[aux-1]);
	}
	free(raw_file);

	for(int i = 0; i < (city_count*city_count); ++i)
		printf("%d ", distances[i]);
	return distances;
}

int get_cost(int* values, int n_cities, int city_a, int city_b) {
	return values[((n_cities*city_b)+city_a)];
}


int main(int argc, char *argv[]) {

	if(argc < 3)
		printf("Usage: (string) FILE.CSV, (integer) index of city to start traveling, (integer) index of city to end traveling\n");

	int *distances = NULL; /* a array that represents the distances between cities, serialized form */
	char **city_names = NULL; /* array with city names*/

	/* loads the city names and returns the number of cities loaded*/
	
	int n_cities = get_n_cities(argv[1]);
	city_names = get_city_names(argv[1], n_cities);
	/* gets the cities costs for travelling */
	distances = get_distances_vecotr(argv[1],n_cities);


	free(distances);
	free(city_names);
}

#ifdef __cplusplus
}
#endif