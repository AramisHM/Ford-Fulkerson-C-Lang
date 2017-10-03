/*  Ford-Fulkerson in C-Lang
	Released under the MIT License
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ford_fulkerson_graph {
	int max_capacity; /* the actual answer for the problem */
	int *edges_capacities; /* the maximum capacity of each edge*/
	int *edges_flow; /* the flow we will be determinating during the execution of the methode of Ford-Fulkerson*/
	int *vertices_visited; /* helps us navigate and determinate some valid paths, prevents infinite ciclic navigation */
	char **vertices; /* array with vertice names*/

	int n_vertices;
	int max_n_edges;

	// these are the variables we use to build paths with the Ford-Fulkerson methode
	int *current_path; // maximum size is the number of vertices in the graph
	int current_path_size; // dis indicates the precedence of nodes we visited
}ford_fulkerson_graph;

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

/*	loads all verticies from the given file to parse into the verticies array
	returns the number of verticies loaded */
int get_n_vertices(char* file_to_parse) {
	char *raw_file;
	long fs = ahmlu_get_file_size(file_to_parse);
	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	/* get verticies names */
	char *verticies = strtok(raw_file,"\n"); /*first line*/
	char *vertice_name = strtok(verticies,"\",");
	
	int vertice_count = 1;

	while (vertice_name) {
		vertice_name = strtok(NULL,"\",");
		if (vertice_name &&
			vertice_name[0] != ',' &&
			vertice_name[0] != '\r')
			++vertice_count;
	}
	free(raw_file);
	return vertice_count;
}

/* parse file and get the vertice names*/
char **get_vertice_names(char *file_to_parse, int vertice_count) {
	char *raw_file;
	char **vertice_names = (char**)malloc(sizeof(char*)*vertice_count);
	
	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	int aux= 0;
	char *verticies = strtok(raw_file,"\n"); /*first line*/
	char *vertice_name = strtok(verticies,"\",");
	while(vertice_name) {
		if (vertice_name &&
			vertice_name[0] != ',' &&
			vertice_name[0] != '\r') {
			vertice_names[aux] = (char*)malloc(sizeof(char)*(strlen(vertice_name)+1));
			memccpy(vertice_names[aux], vertice_name, 1, strlen(vertice_name));
			vertice_names[aux][strlen(vertice_name)]=0;
			++aux;
		}
		vertice_name = strtok(NULL,"\",");
	}

	free(raw_file);
	/* debug: print vertice names */
	for(int i =0; i < vertice_count; ++i)
		printf("%s\n",vertice_names[i]);
	
	return vertice_names;
}

int *get_edges_value_vecotr(char* file_to_parse, int vertice_count) {
	char *raw_file;
	
	int *edges_value = NULL;

	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	edges_value = (int*)malloc(sizeof(int)*(vertice_count*vertice_count));

	strtok(raw_file,"\n\r"); /*ignore first line*/
	char *distance = strtok(NULL,"\n\r, ");
	int aux = 0;
	while (distance) {
		edges_value[aux++] = atoi(distance);
		distance = strtok(NULL,"\n\r, ");
		//printf("%d ",edges_value[aux-1]);
	}
	free(raw_file);

	for(int i = 0; i < (vertice_count*vertice_count); ++i)
		printf("%d ", edges_value[i]);
	return edges_value;
}

int get_cost(int* values, int n_vertices, int vertice_a, int vertice_b) {
	return values[((n_vertices*vertice_a)+vertice_b)];
}

unsigned long factorial(unsigned long f)
{
    if ( f == 0 ) 
        return 1;
    return(f * factorial(f - 1));
}

/* allocates the structures for ford-fulkerson graph*/
ford_fulkerson_graph *create_ford_fulkerson_graph(char *input_file) {
	ford_fulkerson_graph *g = (ford_fulkerson_graph*)malloc(sizeof(ford_fulkerson_graph));
	
	/* loads the vertice names and returns the number of verticies loaded*/
	g->n_vertices = get_n_vertices(input_file);

	g->max_n_edges = factorial(g->n_vertices);

	g->vertices = get_vertice_names(input_file, g->n_vertices);

	/* gets the edges value */
	g->edges_capacities = get_edges_value_vecotr(input_file, g->n_vertices);
	g->edges_flow = (int*)malloc(sizeof(int)*g->max_n_edges);
	g->vertices_visited = (int*)malloc(sizeof(int)*g->n_vertices);
	g->current_path = (int*)malloc(sizeof(int)*g->n_vertices);

	for(int i = 0; i < g->n_vertices; ++i) {
		g->vertices_visited[i]=0; /*initialize as not visited*/
		g->current_path[i] = -1;
	}
	for(int i = 0; i < g->max_n_edges; ++i) {
		g->edges_flow[i]=0; /*initialize as no flow at all*/
	}

	g->max_capacity = 0;
	g->current_path_size = 0;
	return g;
}

void destroy_ford_fulkerson_graph(ford_fulkerson_graph *g) {
	free(g->vertices);
	free(g->edges_capacities);
	free(g->edges_flow);
	free(g->vertices_visited);
	free(g);
}

int *adjacent_vertices(ford_fulkerson_graph *g, int vertex_index, int *n_adja) {
	int *adjacent_vertices = (int*)malloc(sizeof(int)*g->n_vertices);

	for(int i =0; i < g->n_vertices; ++i)
		adjacent_vertices[i] = -1;

	(*n_adja) = 0; // number of adjacent vertices; bidirectional is ignored
	for ( int i = 0; i < g->n_vertices; ++i) {
		int cost_ab = -1;
		int cost_ba = -1;

		if(i == vertex_index)
			continue; // doesnt care about adjacence from self

		cost_ab = get_cost(g->edges_capacities, g->n_vertices, vertex_index, i);
		cost_ba = get_cost(g->edges_capacities, g->n_vertices, i, vertex_index);

		if(cost_ab > 0 || cost_ba > 0) {
			adjacent_vertices[(*n_adja)] = i; // indicate in this vector that this element (OF INDEX I) is adjacent to (vertex_index)
			++(*n_adja); // increment the number of adjacent elements we found
		}
	}
	return adjacent_vertices;
}

/* returns 1 if the flow from a to b exists. otherwise return 0 if is reverse flow. Returns -1 if error. */
int isForwardFlow(ford_fulkerson_graph *g, int a, int b) {
	return (get_cost(g->edges_capacities,g->n_vertices,a,b)>=0?1:0);
}

void ford_fulkerson(ford_fulkerson_graph *g, int s, int t) {
	
	if(s != t) { // while we havent reached the final destination
		int adj_vert_count;
		int *adj_vert = adjacent_vertices(g, s, &adj_vert_count); // get adjacent nodes to s

		g->vertices_visited[s] = 1; // mark as visited

		// debug print adjacent vertices
		//for(int i = 0; i < adj_vert_count; ++ i)
		//	printf("%d\n", adj_vert[i]);

		// continue laying the path through adjacent nodes that haven't been visited
		for(int i = 0; i < adj_vert_count; ++ i) { 
			if(!g->vertices_visited[adj_vert[i]]) { // if this adj vert wasnt visited, lets continue through it
				

				g->current_path[g->current_path_size] = adj_vert[i];
				++g->current_path_size;

				ford_fulkerson(g,adj_vert[i], t);
				//g->vertices_visited[adj_vert[i]] = 0; // unmark as visited

				g->current_path[g->current_path_size] = -1; // make path available again
				--g->current_path_size;
			}
			
		}

		g->vertices_visited[s] = 0; // mark as unvisited

		free (adj_vert);
	} else {
		printf("\n\n");
		for(int i = 0; i < g->current_path_size; ++i) {
			printf("%d, ", g->current_path[i]);
		}
		
	}
}

int main(int argc, char *argv[]) {

	if(argc < 4)
		printf("Usage: (string) FILE.CSV, (integer) index of vertice to start traveling, (integer) index of vertice to end traveling\n");

	ford_fulkerson_graph *g = create_ford_fulkerson_graph(argv[1]);
	
	ford_fulkerson(g, atoi(argv[2]), atoi(argv[3]));

	destroy_ford_fulkerson_graph(g);
}

#ifdef __cplusplus
}
#endif