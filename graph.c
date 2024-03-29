/* Graph operations designed to interface with main.c,
   Created by Constantinos Kavadias, Student ID: 664790
   ckavadias@student.unimelb.edu.au
*/
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "graph.h"

#define ZERO 0
#define DEBUG 0

/* test if there are existing edges listed */
void check_and_link(List* first, Vertex second);

/* add out edge */
void link_edge(List* from, Vertex to);

/*check a pointer has been successful in malloc acquisition*/
void check_malloc(void* pointer);

/* Returns a pointer to a new graph with order vertices */
Graph new_graph(int order) {
    Graph graph;
    
    graph = (Graph)malloc(sizeof(struct graph_t));
    check_malloc(graph);
    graph->vertices = (Vertex)malloc(order*sizeof(struct vertex_t));
    check_malloc(graph->vertices);
    graph->order = order;
    graph->size = ZERO;
    
    return graph;
}

/* Returns whether aim and vertex are pointing to the same location */
bool ptr_eq(void *aim, void *vertex) {
    return false;
}

/* Returns whether aim and vertex have the same id */
bool id_eq(void *aim, void *vertex) {
    return false;
}

/* Add the edge from v1 to v2 to graph */
void add_edge(Graph graph, int v1, int v2) {
	Vertex vertex_1, vertex_2;
	if(DEBUG){fprintf(stderr, "add edge initiated\n");};
	
	vertex_1 = (graph->vertices)+v1;
	vertex_2 = (graph->vertices)+v2;
	
	if(DEBUG){fprintf(stderr, "pre check and link\n");};
	
	/*check if there are existing edges and determine where to add new edge*/
	check_and_link(&(vertex_1->out), vertex_2);
	check_and_link(&(vertex_2->in), vertex_1);
	
	return;
}

/* add out edge */
void link_edge(List* from, Vertex to){
	
	if(DEBUG){fprintf(stderr, "link edge initates\n");};
	 *from = (List)malloc(sizeof(struct list_t));
	 check_malloc(*from);
	(*from)->data = to;
	(*from)->next = NULL;
	
	return;
}

/* test if there are existing edges listed */
void check_and_link(List* first, Vertex second){
	List *current, *next;
	
	if(DEBUG){fprintf(stderr, "check and link initates\n");};
	/* check if this is the first edge to be linked */
	if (*first == NULL){
		if(DEBUG){fprintf(stderr, "pre link edge\n");};
		link_edge(first, second);
	}
	/*if other edges exist, find end of list and append*/
	else{
		if(DEBUG){fprintf(stderr, "pre link edge\n");};
		current = first;
		next = &((*current)->next);
		while( *next != NULL){
			current = next;
			next = &((*current)->next);
		}
		link_edge(next, second);
	}
	return;
}

/* free memory allocated to a vertex */
void free_vertex(Vertex vertex){
	free_list(vertex->in);
	free_list(vertex->out);
	free(vertex->label);
}
/* Free the memory allocated to graph */
void free_graph(Graph graph) {
	int i;
	
	for (i = 0; i < graph->order; i++){
		free_vertex(graph->vertices + i);
	}
	free(graph->vertices);
	free(graph);
}
