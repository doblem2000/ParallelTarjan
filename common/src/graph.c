/* 
 * Course: High Performance Computing 2022/2023 
 * 
 * Lecturer: Francesco Moscato  fmoscato@unisa.it 
 *
 * Group:
 * Langella	Antonio   0622702011   a.langella31@studenti.unisa.it
 * Marsico Michele   0622702012   m.marsico10@studenti.unisa.it
 * Paolino Salvatore   0622702016   s.paolino6@studenti.unisa.it 
 * Risi Davide   0622702013   d.risi2@studenti.unisa.it
 * 
 * Copyright (C) 2023 - All Rights Reserved 
 *
 * This file is part of Parallelize-Tarjan-MPI-CUDA. 
 *
 * Parallelize-Tarjan-MPI-CUDA is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version. 
 *
 * Parallelize-Tarjan-MPI-CUDA is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 *
 * You should have received a copy of the GNU General Public License 
 * along with Parallelize-Tarjan-MPI-CUDA.  If not, see <http://www.gnu.org/licenses/>. 
 * 
 */

/**
 * @file graph.c
 * @brief This file implements the abstract data type graph with adjacency maps. It also contains basic operation on the graph (vertices and edges insert and deletion) as well as specific operations needed by the * * MPI and CUDA algorithms. The file also defines the scc_set data structure which is used to store SCCs found by Tarjan's algorithm as well as operations on this data structure.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graph.h"
#include "khash.h"
#include "linkedlist.h"
#include "random.h"



/*struct graph_t {
    int n_vertex;
    khash_t(mm32) *adj; //Hash table which maps a node with the forward adjacency list
    khash_t(mm32) *inverted_adj; //Hash table which maps a node with the backwards adjacency list
};*/
/**
 * @brief This function takes two integers as input and returns the minimum integer.
 * 
 * @param a first integer.
 * @param b second integer.
 * @return minimum.
 */
int min(int a, int b){
    return a<b ? a:b;
}

/**
 * @brief This function declares and initializes an empty graph_t data structure.
 * 
 * @return graph_t* 
 */
graph_t *graph_init(){
    graph_t *G = (graph_t *) malloc(sizeof(graph_t));
    G->n_vertex=0;
    G->adj = kh_init(mm32);
    G->inverted_adj = kh_init(mm32);
    return G;
}

/**
 * @brief This function takes as input a graph_t data structure and takes care of deallocating the entire memory occupied by the data structure.
 * 
 * @param G graph data structure to be deallocated.
 */
void graph_free(graph_t *G){
    kh_destroy(mm32, G->adj);
    kh_destroy(mm32, G->inverted_adj);
    free(G);
}

/**
 * @brief This function takes as input a graph_t data structure and returns the number of vertices in the graph.
 * 
 * @param G graph data structure of which we want to know the number of vertices.
 * @return int.
 */
int graph_get_num_vertex(graph_t *G){
    return G->n_vertex;
}

/**
 * @brief This function takes as input a graph_t data structure and a vertex v; it takes care of inserting the vertex v within the graph.
 * 
 * @param G graph data structure.
 * @param v vertex to be inserted.
 */
void graph_insert_vertex(graph_t *G, int v){
    int ret;
    khint_t k;

    k = kh_put(mm32,G->adj,v,&ret); //ret == 0 if v is already present in the ht, ret>0 otherwise
    if(ret == 0)//if v was already present in the ht
        return;
    
    (G->n_vertex)++;
    kh_value(G->adj, k) = kh_init(m32); //create a new adjacency map for the node v
    //repeat the same for inverted_adj
    k = kh_put(mm32,G->inverted_adj,v,&ret); //ret == 0 if v is already present in the ht, ret>0 otherwise
    kh_value(G->inverted_adj, k) = kh_init(m32); //create a new adjacency map for the node v
}
/**
 * @brief This function takes as input a graph_t data structure and two vertices u and v. It takes care of inserting an edge from vertex u to vertex v of the graph.
 * \n Throws an error is the edge already exists.
 * 
 * @param G graph data structure.
 * @param u vertex of the graph.
 * @param v vertex of the graph.
 */
void graph_insert_edge(graph_t *G, int u, int v){
    khint_t k;
    bool is_present;
    int ret;

    k = kh_get(mm32, G->adj, u);
    is_present = (k != kh_end(G->adj));
    assert(is_present); //assert u exists
    khash_t(m32) *adj_list = kh_value(G->adj,k);
    k = kh_put(m32, adj_list, v, &ret);//ret == 0 if v is already present in the ht, ret>0 otherwise
    assert(ret != 0); //assert edge didn't exist
    kh_value(adj_list,k) = 42; //TODO: convert map to set

    k = kh_get(mm32, G->inverted_adj, v);
    is_present = (k != kh_end(G->inverted_adj));
    assert(is_present); //assert v exists
    adj_list = kh_value(G->inverted_adj,k);
    k = kh_put(m32, adj_list, u, &ret);//ret == 0 if u is already present in the ht, ret>0 otherwise
    assert(ret != 0); //assert edge didn't exist
    kh_value(adj_list,k) = 42; //TODO: convert map to set
}

/*
 * Deletes an edge from node u to node v if it exists. Otherwise does nothing.
 */

/**
 * @brief This function takes as input a graph_t data structure and two vertices u and v. It deletes the edge from the node u to the node v if it exists. Otherwise does nothing.
 * 
 * @param G graph data structure.
 * @param u vertex of the graph.
 * @param v vertex of the graph.
 */
void graph_delete_edge(graph_t *G, int u, int v){
    khint_t k;
    bool is_present;
    khash_t(m32) *adj_list;
    k = kh_get(mm32, G->adj, u);
    is_present = (k != kh_end(G->adj));
    if(is_present){
        adj_list = kh_value(G->adj,k); //Take the adjacency list
        k = kh_get(m32, adj_list, v);
        kh_del(m32,adj_list,k); //delete the u->v edge from outcoming edges of u
    }
    //Do the same for inverted_adj
    k = kh_get(mm32, G->inverted_adj, v);
    is_present = (k != kh_end(G->inverted_adj));
    if(is_present){
        adj_list = kh_value(G->inverted_adj,k); //Take the adjacency list
        k = kh_get(m32, adj_list, u);
        kh_del(m32,adj_list,k); //delete the u->v edge from incoming edges of v
    }
}

/**
 * @brief This function takes as input a graph_t data structure and a vertex v. It deletes the vertex v and every edge incident on the vertex.
 * 
 * @param G input graph
 * @param v vertex to be deleted
 */
void graph_delete_vertex(graph_t *G, int v){
    int u, _; (void) _; //_ is a needed unused variable variable. We do this to silence -Wunused-but-set-variable warning
    khint_t k;
    khash_t(m32) *adj_list, *inv_adj_list, *temp;
    k = kh_get(mm32,G->adj,v);
    adj_list = kh_value(G->adj,k); //adj_list contains all the outcoming edges of v (v->*)
    k = kh_get(mm32,G->inverted_adj,v);
    inv_adj_list = kh_value(G->inverted_adj,k); //inv_adj_list contains all the incoming edges of v (*->v)

    //For each v->u edge delete the incoming edge of u
    kh_foreach(adj_list, u, _, { 
        k = kh_get(mm32,G->inverted_adj,u);
        temp = kh_value(G->inverted_adj,k);
        k = kh_get(m32, temp,v);
        kh_del(m32, temp, k);
    });

    //For each u->v edge delete the outcoming edge of u
    kh_foreach(inv_adj_list, u, _, { 
        k = kh_get(mm32,G->adj,u);
        temp = kh_value(G->adj,k); //Temp contains all the outcoming edges of u
        k = kh_get(m32, temp,v);
        kh_del(m32, temp, k);
    });

    //Dealloc adj_list and inv_adj_list
    kh_destroy(m32, adj_list);
    kh_destroy(m32, inv_adj_list);

    //Delete v from G->adj and G->inverted_adj
    k = kh_get(mm32, G->adj, v);
    kh_del(mm32, G->adj, k);
    k = kh_get(mm32, G->inverted_adj, v);
    kh_del(mm32, G->inverted_adj, k);
}

/**
 * @brief This function is an helper function for graph_tarjan(). For more info see documentation for graph_tarjan().
 * 
 * @param G graph data structure.
 * @param node pointer to an integer.
 * @param stack stack data structure.
 * @param time pointer to an integer.
 * @param result array data structure.
 */
void graph_tarjan_helper(graph_t *G, int node, khash_t(m32) *disc, khash_t(m32) *low,
   linkedlist_int *stack, khash_t(m32) *stackMember,int *time, array_int *result){ 
    khint_t k, j;
    int adj_node, _; (void) _; //_ is a needed unused variable variable. We do this to silence -Wunused-but-set-variable warning
    // Initialize discovery time and low value
    ++(*time);
    k = kh_get(m32,disc,node);
    kh_value(disc,k) = *time;
    k = kh_get(m32,low,node);
    kh_value(low,k) = *time;

    linkedlist_int_push(stack, node);
    k = kh_get(m32,stackMember,node);
    kh_value(stackMember,k) = 1; //true

    // Go through all vertices adjacent to this
    k = kh_get(mm32,G->adj,node);
    khash_t(m32) *adjacency_list = kh_value(G->adj,k);
    kh_foreach(adjacency_list, adj_node, _, {
        k = kh_get(m32,disc,adj_node);
        //TODO: the above operation should fail when tarjan is executed on subgraphs of a given graph. Add a check.
        //^This is the point where one could decide to ignore absent vertices or even store them to optimize the algorithm later.
        if( kh_value(disc,k) == -1 ){
            graph_tarjan_helper(G, adj_node, disc, low, stack, stackMember, time, result);

            // Check if the subtree rooted with 'v' has a
            // connection to one of the ancestors of 'u'
            // Case 1 (per above discussion on Disc and Low value)
            k = kh_get(m32,low,node);
            j = kh_get(m32,low,adj_node);
            kh_value(low,k) = min(kh_value(low,k),kh_value(low,j));
        }
        // Update low value of 'u' only of 'v' is still in stack
        // (i.e. it's a back edge, not cross edge).
        // Case 2 (per above discussion on Disc and Low value)
        else if (kh_value(stackMember,kh_get(m32,stackMember,adj_node)) == 1){
            k = kh_get(m32,low,node);
            j = kh_get(m32,disc,adj_node);
            kh_value(low,k) = min(kh_value(low,k),kh_value(disc,j));
        }
    });
 
    // head node found, pop the stack and print an SCC
    int w = 0; // To store stack extracted vertices
    k = kh_get(m32,low,node);
    j = kh_get(m32,disc,node);
    if (kh_value(low,k) == kh_value(disc,j)){
        while (linkedlist_int_length(stack) > 0 && linkedlist_int_top(stack) != node){
            w = (int) linkedlist_int_pop(stack);
            k = kh_get(m32,stackMember,w);
            kh_value(stackMember, k) = 0; //false
            array_int_push(result, w);
        }
        w = (int) linkedlist_int_pop(stack);
        k = kh_get(m32,stackMember,w);
        kh_value(stackMember, k) = 0; //false
        array_int_push(result, w);
        array_int_push(result,-1); //SCC terminator
    }
}

/**
 * @brief This function is a Tarjan's algorithm implementation using recursion.
 * \n This is a modified version of the algorithm on the geeksforgeeks.com website.
 * \n The main differences are:
 * \n - disc, low and stackMember are now hash tables because we remove the hypotesis that vertex ids go from 0 to N-1:
 * \n  when working on a subgraph (as a slave process), there are no guarantees on the order nor continuity of the vertex ids.
 * \n  Using hash tables instead of arrays we save a lot of memory.
 * \n - we remove the hypotesis that every vertex in an adjacency map exists in the graph. This is also caused by executions
 * \n on subgraphs of a given graph.
 * 
 * @param G graph data structure.
 * @return array_int* array containing all the found SCCs.
 */
array_int *graph_tarjan(graph_t *G){
    khash_t(m32) *disc = kh_init(m32);
    khash_t(m32) *low = kh_init(m32);
    khash_t(m32) *stackMember = kh_init(m32);
    linkedlist_int *stack = linkedlist_int_init();
    int time = 0;
    array_int *result = array_int_init((G->n_vertex)*2);

    int node,_; (void) _; //_ is a needed unused variable variable. We do this to silence -Wunused-but-set-variable warning
    khash_t(m32) *temp; (void) temp; //temp is a needed unused variable variable. We do this to silence -Wunused-but-set-variable warning
    khint_t k;
    kh_foreach(G->adj, node, temp, {
        k = kh_put(m32,disc,node,&_);
        kh_value(disc,k) = -1;
        k = kh_put(m32,low,node,&_);
        kh_value(low,k) = -1;
        k = kh_put(m32,stackMember,node,&_);
        kh_value(stackMember,k) = 0; //false
    });
    kh_foreach(G->adj, node, temp, {
        k = kh_get(m32,disc,node);
        if(kh_value(disc,k) == -1)
            graph_tarjan_helper(G,node, disc, low, stack, stackMember, &time, result);
    });

    kh_destroy(m32,disc);
    kh_destroy(m32,low);
    kh_destroy(m32,stackMember);
    return result;
}
/**
 * @brief This function is an helper function for graph_tarjan_foreach(). For more info see documentation for graph_tarjan_foreach().
 * 
 * @param G graph data structure.
 * @param node integer which represents the node.
 * @param stack stack data structure.
 * @param time pointer to an integer.
 * @param scc array containing the SCCs.
 * @param f callback function.
 */
void graph_tarjan_foreach_helper(graph_t *G, int node, khash_t(m32) *disc, khash_t(m32) *low,
   linkedlist_int *stack, khash_t(m32) *stackMember,int *time, array_int *scc, void (*f)(array_int *)){ 
    khint_t k, j;
    int adj_node, _; (void) _; //_ is a needed unused variable variable. We do this to silence -Wunused-but-set-variable warning
    // Initialize discovery time and low value
    ++(*time);
    k = kh_get(m32,disc,node);
    kh_value(disc,k) = *time;
    k = kh_get(m32,low,node);
    kh_value(low,k) = *time;

    linkedlist_int_push(stack, node);
    k = kh_get(m32,stackMember,node);
    kh_value(stackMember,k) = 1; //true

    // Go through all vertices adjacent to this
    k = kh_get(mm32,G->adj,node);
    khash_t(m32) *adjacency_list = kh_value(G->adj,k);
    kh_foreach(adjacency_list, adj_node, _, {
        k = kh_get(m32,disc,adj_node);
        //TODO: the above operation should fail when tarjan is executed on subgraphs of a given graph. Add a check.
        //^This is the point where one could decide to ignore absent vertices or even store them to optimize the algorithm later.
        if( kh_value(disc,k) == -1 ){
            graph_tarjan_foreach_helper(G, adj_node, disc, low, stack, stackMember, time, scc, f);

            // Check if the subtree rooted with 'v' has a
            // connection to one of the ancestors of 'u'
            // Case 1 (per above discussion on Disc and Low value)
            k = kh_get(m32,low,node);
            j = kh_get(m32,low,adj_node);
            kh_value(low,k) = min(kh_value(low,k),kh_value(low,j));
        }
        // Update low value of 'u' only of 'v' is still in stack
        // (i.e. it's a back edge, not cross edge).
        // Case 2 (per above discussion on Disc and Low value)
        else if (kh_value(stackMember,kh_get(m32,stackMember,adj_node)) == 1){
            k = kh_get(m32,low,node);
            j = kh_get(m32,disc,adj_node);
            kh_value(low,k) = min(kh_value(low,k),kh_value(disc,j));
        }
    });
 
    // head node found, pop the stack and print an SCC
    int w = 0; // To store stack extracted vertices
    k = kh_get(m32,low,node);
    j = kh_get(m32,disc,node);
    if (kh_value(low,k) == kh_value(disc,j)){
        while (linkedlist_int_length(stack) > 0 && linkedlist_int_top(stack) != node){
            w = (int) linkedlist_int_pop(stack);
            k = kh_get(m32,stackMember,w);
            kh_value(stackMember, k) = 0; //false
            array_int_push(scc, w);
        }
        w = (int) linkedlist_int_pop(stack);
        k = kh_get(m32,stackMember,w);
        kh_value(stackMember, k) = 0; //false
        array_int_push(scc, w);
        //SCC completed
        if( array_int_length(scc) > 1 ){
            f(scc); //callback
        }
        //Clear the scc
        array_int_clear(scc);
    }
}

/**
 * @brief This function takes as input a graph and a callback function f. It finds all the SCCs in the graph and each time it finds one it calls the callback function f on the SCC. 
 * 
 * @param G graph data structure.
 * @param f callback function.
 */
void graph_tarjan_foreach(graph_t *G, void (*f)(array_int *)){
    khash_t(m32) *disc = kh_init(m32);
    khash_t(m32) *low = kh_init(m32);
    khash_t(m32) *stackMember = kh_init(m32);
    linkedlist_int *stack = linkedlist_int_init();
    int time = 0;
    array_int *scc = array_int_init((G->n_vertex));

    int node,_; (void) _; //_ is a needed unused variable variable. We do this to silence -Wunused-but-set-variable warning
    khash_t(m32) *temp; (void) temp; //temp is a needed unused variable variable. We do this to silence -Wunused-but-set-variable warning
    khint_t k;
    kh_foreach(G->adj, node, temp, {
        k = kh_put(m32,disc,node,&_);
        kh_value(disc,k) = -1;
        k = kh_put(m32,low,node,&_);
        kh_value(low,k) = -1;
        k = kh_put(m32,stackMember,node,&_);
        kh_value(stackMember,k) = 0; //false
    });
    kh_foreach(G->adj, node, temp, {
        k = kh_get(m32,disc,node);
        if(kh_value(disc,k) == -1)
            graph_tarjan_foreach_helper(G,node, disc, low, stack, stackMember, &time, scc, f);
    });

    kh_destroy(m32,disc);
    kh_destroy(m32,low);
    kh_destroy(m32,stackMember);
    array_int_free(scc);
}

/**
 * @brief This function takes as input a graph, the number of vertices to be serialized n, and a variable to store the reference to the adicence map.
 * \n The function returns an array of integers representing the serialization of the first n vertices of the graph.
 * \n In other words, we go from a representation using pointers to a representation that uses integers only.
 * 
 * @param G graph data structure.
 * @param n number of vertices to be serialized.
 * @param bucket variable to store the reference to the adicence map.
 * @return array_int* array of integers representing the serialization of the first n vertices.
 */
array_int *graph_serialize(graph_t *G, int n, khint_t * bucket){
    array_int *result = array_int_init(G->n_vertex);
    int words = 0, node_a, node_b, _, serialized = 0; (void) _; //_ is a needed unused variable variable. We do this to silence -Wunused-but-set-variable warning
    khash_t(m32) *adj_list;
    array_int_push(result, 0); //Placeholder for number of total words to read after the first one
    array_int_push(result, 0); //Placeholder for number vertexes serialized
    words++;
    khint_t i;
    for(i = *bucket; i != kh_end(G->adj) && serialized < n; ++i){
        if(!kh_exist(G->adj, i)) // STRANGE BEHAVIOUR OF kh_esist. Sometimes it segfaults.
            continue;
        node_a = kh_key(G->adj,i);
        adj_list = kh_value(G->adj,i);
        array_int_push(result,node_a);
        words++;
        kh_foreach(adj_list, node_b, _, {
            array_int_push(result,node_b);
            words++;
        });
        array_int_push(result,-1); //end of adj list
        words++;
        serialized++;
    }
    *bucket = i;
    array_int_set(result, 0, words);
    array_int_set(result, 1, serialized);
    return result;
}

/**
 * @brief This function takes as input an array representing a deserialized graph and a reference to a graph. 
 * \n The function transforms the serialized representation of the graph to a graph_t representation via pointers on which all library operations are defined.
 * 
 * @param G graph data structure.
 * @param buff array representing a deserialized graph.
 */
void graph_deserialize(graph_t *G, array_int *buff){
    //int words = array_int_get(buff,0);
    int n_vertex = array_int_get(buff,1);
    int i = 2, node_a, node_b;
    for(int j = 0; j<n_vertex;j++){
        node_a = array_int_get(buff,i);
        graph_insert_vertex(G,node_a);
        do{
            i++;
            node_b = array_int_get(buff,i);
            if(node_b != -1){
                graph_insert_vertex(G,node_b); //NOTE: this adds nodes that are not owned by the slave to the graph. This is not a problem.
                graph_insert_edge(G,node_a,node_b);
            }
        } while(node_b != -1);
        i++;
    }
}

/**
 * @brief This function takes as input a graph and a string. It serializes the input graph and stores it on a binary file.
 * 
 * @param G graph data structure.
 * @param filename string representing the filename of the output file.
 */
void graph_save_to_file(graph_t *G, char *filename){
    khint_t pos = 0;
    array_int *array = graph_serialize(G, G->n_vertex, &pos);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL){
       printf("Error creating file %s, aborting.\n", filename);   // Program exits if the file pointer returns NULL.
       exit(1);
    }
    fwrite(array_int_get_ptr(array), sizeof(int), array_int_get(array,0)+1, fp);
    fclose(fp);
    array_int_free(array);
}

/**
 * @brief This function takes a string as input. It extracts a serialized graph from a file and returns a graph in the graph_t format.
 * 
 * @param filename string representing the filename of the input file.
 */
graph_t *graph_load_from_file(char *filename){
    FILE *fp = fopen(filename, "r");
    if (fp == NULL){
       printf("Error opening file %s, aborting.\n", filename);   // Program exits if the file pointer returns NULL.
       //exit(1);
       return NULL;
    }
    int n;
    fread(&n, sizeof(int), 1, fp);
    array_int *array = array_int_init(n+1);
    array_int_set(array, 0, n); //Fill position 0
    array_int_resize(array, n+1); //Sets array length = n+1
    fread(array_int_get_ptr(array)+1, sizeof(int), n, fp); //Fill from position 1 to n
    graph_t *graph = graph_init();
    graph_deserialize(graph, array);
    fclose(fp);
    array_int_free(array);
    return graph;
}
/**
 * @brief This function takes as input graph, a vertex identifier 'dest' and an array of vertices. It merges all vertices in the array into the vertex 'dest'.
 * 
 * @param G graph data structure.
 * @param dest a vertex identifier.
 * @param src an array of vertices.
 */
void graph_merge_vertices(graph_t *G, int dest, array_int *src){
    int src_node, cpy, _; (void) _; //_ is a needed unused variable variable. We do this to silence -Wunused-but-set-variable warning
    khash_t(m32) *adj_list_src, *adj_list_dest, *inv_adj_list_src, *inv_adj_list_dest;
    khint_t k;
    
    //Get dest node adjacency list and inverted adjacency list
    k = kh_get(mm32, G->adj, dest);
    adj_list_dest = kh_value(G->adj, k);
    k = kh_get(mm32, G->inverted_adj, dest);
    inv_adj_list_dest = kh_value(G->inverted_adj, k);

    //Merge each src_node into dest node
    for(int i=0; i < array_int_length(src); i++){
        src_node = array_int_get(src, i);
        if(src_node == dest) //If src_node is equal to dest no merge is needed
            continue;
        //Get src_node adjacency list and inverted adjacency list
        k = kh_get(mm32, G->adj, src_node);
        adj_list_src = kh_value(G->adj, k);
        k = kh_get(mm32, G->inverted_adj, src_node);
        inv_adj_list_src = kh_value(G->inverted_adj, k);

        //Copy each node of src_node's adjacency list into dest's. Update inverted_adj accordingly.
        kh_foreach(adj_list_src, cpy, _, {
            if(cpy == src_node) //Copying auto-referencing edges produces errors. We don't do that
                continue;
            kh_put(m32, adj_list_dest, cpy, &_); //We copy the node of adj_list_src to adj_list_dest
            k = kh_get(mm32, G->inverted_adj, cpy);
            kh_put(m32, kh_value(G->inverted_adj, k), dest, &_); //Update inverted_adj accordingly.
        });
        //Copy each node of src_node's inverted adjacency list into dest's. Update adj accordingly.
        kh_foreach(inv_adj_list_src, cpy, _, {
            if(cpy == src_node) //Copying auto-referencing edges produces errors. We don't do that
                continue;
            kh_put(m32, inv_adj_list_dest, cpy, &_); //We copy the node of inv_adj_list_src to inv_adj_list_dest
            k = kh_get(mm32, G->adj, cpy);
            kh_put(m32, kh_value(G->adj, k), dest, &_); //Update adj accordingly.
        });
        //Now it is safe to delete the src vertex
        graph_delete_vertex(G, src_node); //TODO: This can be optimized 
    }
}

 /**
 * @brief Merge 2 graph and the merged graph is in graph_t * to
 * 
 * @param  to     graph with vertex index from 0 to graph_get_num_vertex(to)
 * @param  from   graph with vertex index from 0 to graph_get_num_vertex(from)
 * @param p       probability of create an edge between a node of graph from and a node of graph to  and viceversa
 */
void graph_merge(graph_t *to, graph_t *from, double p){ //give a graph to and a graph from and merge both, return graph is in graph to
    int i=0;
    int initial_number_of_vertex_graph_to=graph_get_num_vertex(to);
    int initial_number_of_vertex_graph_from=graph_get_num_vertex(from);
    int key=0;
    int value=0;
    int opposite=0;
    int to_vertex=0;
    int new_dimension=0;
    khint_t k;
    khash_t(m32) *adj_list;


    for(i=0; i<initial_number_of_vertex_graph_from; i++){
        k = kh_get(mm32, from->adj, i);
        adj_list = kh_value(from->adj, k);     //trovo adiacent list associata al vertice i

        graph_insert_vertex(to, i+initial_number_of_vertex_graph_to);

        kh_foreach(adj_list, key, value, {
            graph_insert_vertex(to, key+initial_number_of_vertex_graph_to);
            graph_insert_edge(to, i+initial_number_of_vertex_graph_to, key+initial_number_of_vertex_graph_to);
        });
        if(rand_bernoulli(p)){
            opposite= rand() % initial_number_of_vertex_graph_to;
            graph_insert_edge(to, i+initial_number_of_vertex_graph_to, opposite);
        }
    }

    new_dimension=graph_get_num_vertex(to);

    for(i=initial_number_of_vertex_graph_to; i<new_dimension; i++){
        if(rand_bernoulli(p)){
            to_vertex=rand() % initial_number_of_vertex_graph_to;
            graph_insert_edge(to, to_vertex, i);
        }
    }
    
}

/**
 * @brief create a random graph with max_n_node node and each node have mean number of edge with a variance_edge
 * 
 * @param max_n_node number of node of graph
 * @param mean_edges mean edge for each node
 * @param variance_edges variance of number of edge for each node 
 * @return graph_t* graph generated
 */
graph_t *graph_random(int max_n_node, int mean_edges, double variance_edges){ 
    int maxNumberOfEdges;
    graph_t *graph;
    int i=0;
    int j=0;
    int opposite=0;
    khint_t k;
    khash_t(m32) *adj_list;

    graph= graph_init();


    for(i=0; i<max_n_node; i++){
        graph_insert_vertex(graph, i);
    }

    for(i=0; i<max_n_node; i++){
        maxNumberOfEdges= rand_binomial_2(mean_edges,variance_edges);
        //printf("numero di archi %d\n", maxNumberOfEdges);
        assert(maxNumberOfEdges < max_n_node);
        j=0;
        while(j<maxNumberOfEdges){
            opposite= rand() % max_n_node ;
            k = kh_get(mm32, graph->adj, i);
            adj_list = kh_value(graph->adj, k);     //trovo adiacent list associata al vertice j

            k=kh_get(m32,adj_list,opposite);    //cerco valore associato alla chiave oppiste (arco associato al nodo opposite)

            if(k == kh_end(adj_list)){          //se edges non è presente lo aggiungo
                graph_insert_edge(graph, i, opposite);
                j++;
            }
        }
    }
    return graph;
}
/**
 * @brief create a graph fully connected or fully disconnected with max_n_node
 * 
 * @param max_n_node number of node of graph
 * @param isFullyConnected 0 create a fully disconneted graph 1 create a fully connected graph
 * @return graph_t* graph generated
 */
graph_t *graph_fully_connected_disconnected(int max_n_node, int isFullyConnected){ 
    graph_t *graph;
    int i=0;
    int j=0;

    graph= graph_init();
    for(i=0; i<max_n_node; i++){
        graph_insert_vertex(graph, i);
    }
    if(isFullyConnected == 1){
        for(i=0; i<max_n_node; i++){
            for(j=0; j<max_n_node; j++){
                graph_insert_edge(graph, i, j);
            }
        }
    }
    return graph;
}

/**
 * @brief geneate a copy of a graph
 * 
 * @param from graph to be copied 
 * @return graph_t* graph generated
 */
graph_t * graph_copy(graph_t * from){
    int i=0;
    int initial_number_of_vertex_graph_from=graph_get_num_vertex(from);
    int key=0;
    int value=0;
    khint_t k;
    khash_t(m32) *adj_list;

    graph_t * to;
    to = graph_init();

    for(i=0; i<initial_number_of_vertex_graph_from; i++){
        graph_insert_vertex(to, i);             //copio tutti i vertici
        
        k = kh_get(mm32, from->adj, i);
        adj_list = kh_value(from->adj, k);     //trovo adiacent list associata al vertice i del grafo from
        
        kh_foreach(adj_list, key, value, {
            graph_insert_vertex(to, key);
            graph_insert_edge(to, i, key);
        });
    }
    return to;
}
/**
 * @brief print on standard output graph in input: number of edged, for earch line node -> adjacency list and node-> inverted adjacency list
 * 
 * @param G graph to be printed
 */
void graph_print_debug(graph_t *G){
    int key;
    khash_t(m32) *value;
    printf("Number of vertices: %d\n", G->n_vertex);
    printf("Adjacency list:\n");
    kh_foreach(G->adj, key, value, {
        printf("%d -->", key);
        for(khint_t i = 0; i != kh_end(value); ++i){
            if(!kh_exist(value, i))
                continue;
            printf(" %d", kh_key(value, i));
        }
        printf("\n");
    });

    printf("Inverted adjacency list:\n");
    kh_foreach(G->inverted_adj, key, value, {
        printf("%d -->", key);
        for(khint_t i = 0; i != kh_end(value); ++i){
            if(!kh_exist(value, i))
                continue;
            printf(" %d", kh_key(value, i));
        }
        printf("\n");
    });
}

struct scc_set_t {
    khash_t(ms32) *scc_map; //The key is the lowest node in the SCC, the value is the set of nodes in the SCC (key included)
    khash_t(m32) *nodes_to_scc_map; //The key is a node of the graph, the value is the SCC where it belongs
};

 /**
  * @brief Initialize a new scc_set
  * 
  * @return scc_set_t* The scc_set
  */
scc_set_t *scc_set_init(){
    scc_set_t *S = (scc_set_t *) malloc(sizeof(scc_set_t));
    S->scc_map = kh_init(ms32);
    S->nodes_to_scc_map = kh_init(m32);
    return S;
}

 /**
  * @brief Destroy an scc_set
  * 
  * @param S The scc_set to be destroyed.
  */
void scc_set_free(scc_set_t *S){
    kh_destroy(ms32, S->scc_map);
    kh_destroy(m32, S->nodes_to_scc_map);
    free(S);
}

 /**
  * @brief Add a new SCC to the set handling merges if needed.
  * 
  * @param S the reference to the scc_set.
  * @param scc_id the id of the SCC to be added. By convention, it is the lowest among the ids of the nodes in the SCC.
  * @param nodes the nodes of the SCC. 
  */
void scc_set_add(scc_set_t *S, int scc_id, array_int *nodes){
    int target_scc_id = scc_id, node, _, node_scc, ret, source_scc_id; (void)_;
    khash_t(s32) *scc_to_merge; //Set of all scc_ids that need to be merged
    khash_t(s32) *target_scc, *source_scc;
    khint_t k;
    
    scc_to_merge = kh_init(s32);
    kh_put(s32, scc_to_merge, scc_id, &_); //scc_id should be merged with other SCCs
    //Find target SCC and SCCs that need to be merged
    for(int i = 0; i < array_int_length(nodes); i++){
        node = array_int_get(nodes, i);
        k = kh_get(m32, S->nodes_to_scc_map, node); 
        if (k == kh_end(S->nodes_to_scc_map)) //If node is not present in an SCC, target_scc_id stays the same
            continue;
        //Otherwise we add the node scc to the set of SCCs to be merged...
        node_scc = kh_value(S->nodes_to_scc_map, k); //The SCC to which node belongs to
        kh_put(s32, scc_to_merge, node_scc, &_);
        //...and we check if target_scc_id should change (it is always the lowest among SCC IDs).
        target_scc_id = min(target_scc_id, node_scc);
    }

    //Get target_scc with id target_scc_id 
    k = kh_put(ms32, S->scc_map, target_scc_id, &ret);
    if(ret!=0){ //If target_scc_id doesn't exist, create it
        kh_value(S->scc_map, k) = kh_init(s32);
    }
    target_scc = kh_value(S->scc_map, k);
    
    //Move every node in nodes to target_scc
    for(int i = 0; i < array_int_length(nodes); i++){
        node = array_int_get(nodes, i);
        //Put the node in the target SCC
        kh_put(s32, target_scc, node, &_);
        //Update nodes_to_scc_map
        k = kh_put(m32, S->nodes_to_scc_map, node, &_);
        kh_value(S->nodes_to_scc_map, k) = target_scc_id;
    }
    
    //Move every node in scc_map[scc_to_merge[*]] to target_scc
    int cpy;
    khint_t source_scc_bucket;
    for(khint_t i = 0; i != kh_end(scc_to_merge); ++i){
        if(!kh_exist(scc_to_merge, i))
            continue;
        source_scc_id = kh_key(scc_to_merge, i);
        if(source_scc_id == target_scc_id) //If the source SCC is the same as target SCC there is nothing to move
            continue;
        source_scc_bucket = kh_get(ms32, S->scc_map, source_scc_id);
        if(source_scc_bucket == kh_end(S->scc_map)) //The source SCC could not exist because scc_id passed by the client could not exist
            continue;
        source_scc = kh_value(S->scc_map, source_scc_bucket);
        for(khint_t j = 0; j != kh_end(source_scc); ++j){ //Put every element of source_scc in target SCC and update nodes_to_scc_map
            if(!kh_exist(source_scc, j))
                continue;
            cpy = kh_key(source_scc, j);
            //Put the node in the target SCC
            kh_put(s32, target_scc, cpy, &_); 
            //Update nodes_to_scc_map
            k = kh_put(m32, S->nodes_to_scc_map, cpy, &_);
            kh_value(S->nodes_to_scc_map, k) = target_scc_id;
        }
        //Now we can delete the SCC with id source_scc_id which is in bucket source_scc_bucket
        kh_destroy(s32, source_scc);
        kh_del(ms32, S->scc_map, source_scc_bucket);
    }
    kh_destroy(s32, scc_to_merge);
}

 /**
  * @brief print an scc_set
  * 
  * @param S ssc_set to be printed
  */
void scc_set_print_debug(scc_set_t *S){
    int key, value_i;
    khash_t(s32) *value;
    printf("SCC MAP:\n");
    kh_foreach(S->scc_map, key, value, {
        printf("%d -->", key);
        for(khint_t i = 0; i != kh_end(value); ++i){
            if(!kh_exist(value, i))
                continue;
            printf(" %d", kh_key(value, i));
        }
        printf("\n");
    });
    printf("NODE TO SCC MAPPINGS:\n");
    kh_foreach(S->nodes_to_scc_map, key, value_i, {
        printf("%d --> %d\n", key, value_i);
    });
    
}

 /**
  * @brief Merge src scc_set into dest.
  * 
  * @param dest the reference of the destination scc_set.
  * @param src the reference of the source scc_set.
  */
void scc_set_merge(scc_set_t *dest, scc_set_t *src){
    int key, value_i;
    khash_t(s32) *value;
    array_int *temp_nodes = array_int_init(25);
    kh_foreach(src->scc_map, key, value, {
        printf("key: %d",key);
        array_int_clear(temp_nodes);
        array_int_push(temp_nodes, key);
        for(khint_t i = 0; i != kh_end(value); ++i){
            if(!kh_exist(value, i))
                continue;
            printf("value: %d",kh_key(value, i));
            
            array_int_push(temp_nodes, kh_key(value, i));
        }
        printf("\nAdding:\n");
        array_int_print(temp_nodes);
        scc_set_add(dest, key, temp_nodes);
    });
    array_int_free(temp_nodes);
}

 /**
  * @brief Check if scc set b contains all of scc set a's content.
  * 
  * @param b the first scc_set.
  * @param a the second scc_set.
  * @return true scc set b contains all of scc set a's content.
  * @return false scc set b not contains all of scc set a's content.
  */
bool scc_set_contains(scc_set_t *b, scc_set_t *a){
    int key, value_i;
    khash_t(s32) *value, *temp_scc_set;
    int temp_node;
    bool is_present;
    khint_t k;
    kh_foreach(a->scc_map, key, value, {
        //Check if scc_id 'key' is present in b
        k = kh_get(ms32, b->scc_map, key);
        is_present = (k != kh_end(b->scc_map));
        if(!is_present){
            printf("Key %d not present\n", key);
            return false;
        }
            
        temp_scc_set = kh_value(b->scc_map,k); //If the set is present in b we take a reference to it
        for(khint_t i = 0; i != kh_end(value); ++i){ //For each element in a's set
            if(!kh_exist(value, i))
                continue;
            temp_node = kh_key(value, i); //Temp node is the one we currently check
            k = kh_get(s32, temp_scc_set, temp_node); //Check if temnp node is in b's set
            is_present = (k != kh_end(temp_scc_set));
            if(!is_present){
                printf("Value %d not present in key %d\n", temp_node, key);
                return false;
            }
        }
    });
    return true;
}

array_int *scc_set_serialize(scc_set_t *S){
    array_int *result = array_int_init(1);
    int words = 0, serialized = 0;
    array_int_push(result, 0); //Placeholder for number of total words to read after the first one
    array_int_push(result, 0); //Placeholder for number sccs serialized
    words++;

    int key;
    khash_t(s32) *value;
    kh_foreach(S->scc_map, key, value, {
        array_int_push(result, key);
        words++;
        for(khint_t i = 0; i != kh_end(value); ++i){
            if(!kh_exist(value, i))
                continue;
            array_int_push(result, kh_key(value, i));
            words++;
        }
        array_int_push(result,-1); //end of scc
        words++;
        serialized++;
    });
    array_int_set(result, 0, words);
    array_int_set(result, 1, serialized);
    return result;
}

void scc_set_deserialize(scc_set_t *S, array_int *buff){
    int n_sccs = array_int_get(buff,1);
    int i = 2, scc_id, node;
    array_int *temp = array_int_init(50);
    for(int j=0; j<n_sccs; j++){
        array_int_clear(temp);
        scc_id = array_int_get(buff,i);
        do{
            i++;
            node = array_int_get(buff,i);
            if(node != -1){
                array_int_push(temp, node);
            }
        } while(node != -1);
        scc_set_add(S, scc_id, temp);
        i++;
    }
    array_int_free(temp);
}

 /**
  * @brief Write scc set to file
  * 
  * @param S the scc_set to be saved.
  * @param filename the file to be saved to.
  */
void scc_set_save_to_file(scc_set_t *S, char *filename){
    array_int *array = scc_set_serialize(S);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL){
       printf("Error creating file %s, aborting.\n", filename);   // Program exits if the file pointer returns NULL.
       exit(1);
    }
    fwrite(array_int_get_ptr(array), sizeof(int), array_int_get(array,0)+1, fp);
    fclose(fp);
    array_int_free(array);
}

 /**
  * @brief Load scc set from file
  * 
  * @param filename the file to load the set from.
  * @return scc_set_t* ssc set readed
  */
scc_set_t *scc_set_load_from_file(char *filename){
    FILE *fp = fopen(filename, "r");
    if (fp == NULL){
       printf("Error opening file %s, aborting.\n", filename);   // Program exits if the file pointer returns NULL.
       //exit(1);
       return NULL;
    }
    int n;
    fread(&n, sizeof(int), 1, fp);
    array_int *array = array_int_init(n+1);
    array_int_set(array, 0, n); //Fill position 0
    array_int_resize(array, n+1); //Sets array length = n+1
    fread(array_int_get_ptr(array)+1, sizeof(int), n, fp); //Fill from position 1 to n
    scc_set_t *S = scc_set_init();
    scc_set_deserialize(S, array);
    fclose(fp);
    array_int_free(array);
    return S;
}

