/*
   Toposort methods adhering to Kahn's Algorithm and
   DFS algorithm designed to interface with main.c,
   Created by Constantinos Kavadias, Student ID: 664790
   ckavadias@student.unimelb.edu.au
*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "toposort.h"
 
#define DEBUG 0

typedef struct {
	bool visited;
	bool visiting;
} visit_t;

/*check a pointer has been successful in malloc acquisition*/
void check_malloc(void* pointer);

/* prints defined debug message if debugging is on */
void debug(char* message){
	if (DEBUG){
		fprintf(stderr, "%s\n", message);
	}
}

/*print vertex label */
void print_v(FILE *f, void* data){
	fprintf(f, "%s\n", ((Vertex)data)->label);
}

/*print list when debugging is on */
void debug_print(FILE* f, List list){
	if(DEBUG){
		print_list(print_v, f, list);
	}
}
/* recursively performs DFS method to produce list of toposorted vertices */
void dfs_recursive(Vertex vertex, List* dfs_list, visit_t* visits){
	List dependant = NULL;
	
	if(visits[vertex->id].visiting){
		fprintf(stderr, "Cycle detected, toposort impossible");
		exit(EXIT_FAILURE);
	}
	if(!visits[vertex->id].visited){
		visits[vertex->id].visiting = true;
		dependant = vertex->out;
		while (dependant){
			dfs_recursive((Vertex)dependant->data, dfs_list, visits);
			dependant = dependant->next;
		}
		visits[vertex->id].visiting = false;
		visits[vertex->id].visited = true;
		*dfs_list = push(*dfs_list, vertex);
	}
}

/* sets all values of visit_t array to an initial false */
void set_false(visit_t* visits, int order){
	int i;
	
	for ( i = 0; i < order; i++){
		visits[i].visiting = false;
		visits[i].visited = false;
	}
	return;
}

/* compile a list of vertices with no incomign edges if none exit with failure 
	as toposort can't be completed*/
List no_incoming(Graph graph){
	int i;
	List no_in = NULL;
	
	/*loop to find all non incoming vertices and store in linked list*/
	for ( i = 0; i < graph->order; i++){
		if (!(graph->vertices)[i].in){
			if(!no_in){
				no_in = push(NULL, (graph->vertices) + i);
			}
			else{
				prepend(&no_in, graph->vertices + i);
			}
		}
	}
	if( !no_in ){
		fprintf(stderr, "All vertices have incoming edges, toposort abort");
		exit(EXIT_FAILURE);
	}
	return no_in;
}

/* Returns a list of topologically sorted vertices using the DFS method */
List dfs_sort(Graph graph) {
	List* dfs_list = NULL;
	visit_t* visits = NULL;
	int i;
	
	dfs_list = (List*)malloc(sizeof(List));
	check_malloc(dfs_list);
	*dfs_list = NULL;
	
	/*initialise auxillary array of visit status for all vertices*/
	visits  = (visit_t*)malloc((graph->order)*sizeof(visit_t));
	check_malloc(visits);
	set_false(visits, graph->order);
	
	/*check for unvisited edges and visit*/
	for(i = 0; i < graph->order; i++){
		if(!visits[i].visited){
			dfs_recursive(graph->vertices + i, dfs_list, visits);
		}
	}
	free(visits);
	visits = NULL;
	
    return *dfs_list;
}

/* delete an edge */
void delete_edge(void* data, List* list){
	List previous = NULL, current = NULL;
	
	if(!(*list)){
		fprintf(stderr, "Deletion error, no list, program aborted\n");
		exit(EXIT_FAILURE);
	}
	
	debug("assignment attempted");
	previous = *list;
	current = previous->next;
	
	/*if data at head of list pop and return */
	debug("Check initial element");
	if(previous->data == data){
		pop(list);
		return;
	}
	
	/*loop through rest of list to find data and cut out*/
	debug("Checking greater list");
	while(current){
		if(current->data == data){
			previous->next = current->next;
			free(current);
			current = NULL;
			return;
		}
		previous = current;
		current = current->next;
		
	}
	
	/*if function hasn't yet ended an incorrect call has been made, 
	  termination is required as catastrophic error exists*/
	fprintf(stderr, "Deletion error, program aborted");
	exit(EXIT_FAILURE);
}

/*removes edge from a graph */
void remove_edge(Vertex from, Vertex to){
	debug("remove_edge started");
	debug(to->label);
	delete_edge(from, &(to->in));
	debug("first edge deleted");
	debug(from->label);
	delete_edge(to, &(from->out));
	debug("remove_edge completed");
	return;
}

/*check if a Graph still contains edges after kahn sort is run, exit if so */
void check_edges(Graph graph){
	int i;
	
	/* check if any incoming edges still present and abort toposort if so*/
	for(i = 0; i < graph->order; i++){
		if (graph->vertices[i].in){
			fprintf(stderr, "Incoming edges detected, abort kahn sort\n");
			exit(EXIT_FAILURE);
		}
	}
	return;
}

/* Returns a list of topologically sorted vertices using the Kahn method */
List kahn_sort(Graph graph) {
	List no_in = NULL, kahn_sort = NULL, current = NULL;
	Vertex vertex;
	
	/*create list of vertixes with no incoming edges*/
	no_in = no_incoming(graph);
	
	while(no_in){
		debug("\nCurrent no_in");
		debug_print(stderr, no_in);
		/*create toposort list if not already done */
		if(!kahn_sort){
			kahn_sort = push(kahn_sort, no_in->data);
		}
		/*if toposort list already made, add no incoming vertex to it*/
		else{
			insert(no_in->data, &kahn_sort);
		}
		current = ((Vertex)(no_in->data))->out;
		debug("\n current outgoings");
		debug_print(stderr, current);
		
		/*check all outgoing vertices from current vertex, delete their shared
		   edge, if no other incoming edges for these vertices, add to no_in*/
		while(current){
			debug("\nSecondary While Loop Initiated");
			remove_edge(no_in->data, current->data);
			vertex = current->data;
			current = current->next;
			if(!((Vertex)(vertex))->in){
				insert(vertex, &no_in);
				debug(((Vertex)(vertex))->label);
			}
			
		}
		pop(&no_in);
	}
	/*check for cycles */
	check_edges(graph);
    return kahn_sort;
}

/* compare data pointers stored in lists for equality */
bool is_equal(void* aim, void* node){
	List compare;
	
	compare = (List)node;
	if(compare->data == aim){
		return true;
	}
	return false;
}
/* check if any vertices in proceeding are also in incoming */
bool check_incoming(List proceeding, List incoming){
	List current = NULL;
	
	current = proceeding;
	while(current){
		debug("check_incoming while loop initiated");
		if(find(is_equal, current->data, incoming)){
			return true;
		}
		current = current->next;
	}
	return false;
}

/* Uses graph to verify vertices are topologically sorted */
bool verify(Graph graph, List vertices) {
	List current = NULL;
	int id;
	
	current = vertices;
	
	/*check if any vertices have edges in their in
		list as defined by graph and return false if so*/
	while(current){
		debug("verify while loop initiated");
		id = ((Vertex)(current->data))->id;
		if(check_incoming(current->next, graph->vertices[id].in)){
			return false;
		}
		current = current->next;
	}
    return true;
}
