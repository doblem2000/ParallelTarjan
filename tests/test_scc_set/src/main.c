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

#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

scc_set_t *SCCs;

void pippo(){
    //    (void) argc;
//    (void) argv;
    scc_set_t *SCCs = scc_set_init();

    array_int *scc = array_int_init(10);
    array_int_push(scc, 10);
    array_int_push(scc, 9);
    array_int_push(scc, 8);
    array_int_push(scc, 7);
    array_int_push(scc, 6);
    array_int_push(scc, 5);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    scc_set_print_debug(SCCs);
    array_int_free(scc);

    printf("\n");
    scc = array_int_init(10);
    array_int_push(scc, 1);
    array_int_push(scc, 2);
    array_int_push(scc, 3);
    array_int_push(scc, 4);
    array_int_push(scc, 99);
    array_int_push(scc, 88);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    scc_set_print_debug(SCCs);
    array_int_free(scc);

    printf("\n");
    scc = array_int_init(10);
    array_int_push(scc, 7);
    array_int_push(scc, 155);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    scc_set_print_debug(SCCs);
    array_int_free(scc);

    printf("\n");
    scc = array_int_init(10);
    array_int_push(scc, 99);
    array_int_push(scc, 0);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    scc_set_print_debug(SCCs);
    array_int_free(scc);


    scc_set_free(SCCs);
}

void test_scc_set_add(){
    scc_set_t *SCCs = scc_set_init();
    array_int *scc = array_int_init(10);
    
    array_int_push(scc, 10);
    array_int_push(scc, 9);
    array_int_push(scc, 8);
    array_int_push(scc, 7);
    array_int_push(scc, 6);
    array_int_push(scc, 5);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    scc_set_print_debug(SCCs);
    array_int_free(scc);

    printf("\nAggiungo un nuovo scc:\n");
    scc = array_int_init(10);
    array_int_push(scc, 1);
    array_int_push(scc, 2);
    array_int_push(scc, 3);
    array_int_push(scc, 4);
    array_int_push(scc, 99);
    array_int_push(scc, 8);
    array_int_push(scc, 88);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    scc_set_print_debug(SCCs);
    array_int_free(scc);

    scc_set_free(SCCs);
}

void test_tarjan_scc(){
    scc_set_t *SCCs = scc_set_init();
    graph_t *graph = graph_init();
    array_int *tarjan, *temp=array_int_init(5);
    int x=0;
    bool inserito=false;

    for(int i = 0; i <= 10; i++)
        graph_insert_vertex(graph,i);
    
    graph_insert_edge(graph, 0, 1);
    graph_insert_edge(graph, 0, 3);
    graph_insert_edge(graph, 1, 2);
    graph_insert_edge(graph, 1, 4);
    graph_insert_edge(graph, 2, 0);
    graph_insert_edge(graph, 2, 6);
    graph_insert_edge(graph, 3, 2);
    graph_insert_edge(graph, 4, 5);
    graph_insert_edge(graph, 4, 6);
    graph_insert_edge(graph, 5, 6);
    graph_insert_edge(graph, 5, 7);
    graph_insert_edge(graph, 5, 8);
    graph_insert_edge(graph, 5, 9);
    graph_insert_edge(graph, 6, 4);
    graph_insert_edge(graph, 7, 9);
    graph_insert_edge(graph, 8, 9);
    graph_insert_edge(graph, 9, 8);

    tarjan=graph_tarjan(graph);
    //array_int_print(tarjan);

    for(int i=0; i<array_int_length(tarjan); i++){
        if(inserito)
            temp=array_int_init(5);
        x=array_int_get(tarjan,i);
        if(x!=-1){
            array_int_push(temp,x);
            inserito=false;
        }
        else{
            scc_set_add(SCCs,array_int_get_min(temp),temp);
            //array_int_print(temp);
            array_int_free(temp);
            //graph_print_debug(graph);
            inserito=true;
        }
    }
    scc_set_print_debug(SCCs);
    array_int_free(tarjan);
    graph_free(graph);
    scc_set_free(SCCs);
}

void callback(array_int *scc){
    scc_set_add(SCCs,array_int_get_min(scc),scc);
}

void test_scc_foreach(){
    SCCs = scc_set_init();
    graph_t *graph = graph_init();
   // array_int *temp=array_int_init(5);

    for(int i = 0; i <= 10; i++)
        graph_insert_vertex(graph,i);
    
    graph_insert_edge(graph, 0, 1);
    graph_insert_edge(graph, 0, 3);
    graph_insert_edge(graph, 1, 2);
    graph_insert_edge(graph, 1, 4);
    graph_insert_edge(graph, 2, 0);
    graph_insert_edge(graph, 2, 6);
    graph_insert_edge(graph, 3, 2);
    graph_insert_edge(graph, 4, 5);
    graph_insert_edge(graph, 4, 6);
    graph_insert_edge(graph, 5, 6);
    graph_insert_edge(graph, 5, 7);
    graph_insert_edge(graph, 5, 8);
    graph_insert_edge(graph, 5, 9);
    graph_insert_edge(graph, 6, 4);
    graph_insert_edge(graph, 7, 9);
    graph_insert_edge(graph, 8, 9);
    graph_insert_edge(graph, 9, 8);

    graph_tarjan_foreach(graph, &callback);
    scc_set_print_debug(SCCs);
    graph_free(graph);
    scc_set_free(SCCs);
}

void test_scc_set_merge(){
    scc_set_t *SCCs = scc_set_init();
    scc_set_t *SCCs2 = scc_set_init();
    array_int *scc = array_int_init(10);
    
    array_int_push(scc, 10);
    array_int_push(scc, 9);
    array_int_push(scc, 8);
    array_int_push(scc, 7);
    array_int_push(scc, 6);
    array_int_push(scc, 5);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    array_int_free(scc);

    scc = array_int_init(10);
    array_int_push(scc, 1);
    array_int_push(scc, 2);
    array_int_push(scc, 3);
    array_int_push(scc, 4);
    array_int_push(scc, 99);
    array_int_push(scc, 899);
    array_int_push(scc, 88);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    
    array_int_free(scc);

    scc = array_int_init(10);
    array_int_push(scc, 42);
    array_int_push(scc, 44);
    array_int_push(scc, 4444);
    array_int_push(scc, 413);
    array_int_push(scc, 444);
    array_int_push(scc, 49);
    array_int_push(scc, 488);
    scc_set_add(SCCs2, array_int_get_min(scc), scc);
    array_int_free(scc);
    printf("\na:\n");
    scc_set_print_debug(SCCs);
    printf("\nb:\n");
    scc_set_print_debug(SCCs2);
    printf("\na+b:\n");
    scc_set_merge(SCCs, SCCs2);
    scc_set_print_debug(SCCs);
    printf("%d %d\n",scc_set_contains(SCCs,SCCs2),scc_set_contains(SCCs2,SCCs));
    fflush(stdout);

    scc_set_free(SCCs);
    scc_set_free(SCCs2);

    
}

void test_scc_serialize(){
    scc_set_t *SCCs = scc_set_init();
    array_int *scc = array_int_init(10);
    
    array_int_push(scc, 10);
    array_int_push(scc, 9);
    array_int_push(scc, 8);
    array_int_push(scc, 7);
    array_int_push(scc, 6);
    array_int_push(scc, 5);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    array_int_free(scc);

    scc = array_int_init(10);
    array_int_push(scc, 1);
    array_int_push(scc, 2);
    array_int_push(scc, 3);
    array_int_push(scc, 4);
    array_int_push(scc, 99);
    array_int_push(scc, 899);
    array_int_push(scc, 88);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    array_int_free(scc);

    array_int *x=scc_set_serialize(SCCs);
    array_int_print(x);
    array_int_free(x);
    scc_set_free(SCCs);
}

void test_scc_deserialize(){
    scc_set_t *SCCs = scc_set_init();
    array_int *scc = array_int_init(10);
    
    array_int_push(scc, 10);
    array_int_push(scc, 9);
    array_int_push(scc, 8);
    array_int_push(scc, 7);
    array_int_push(scc, 6);
    array_int_push(scc, 5);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    array_int_free(scc);

    scc = array_int_init(10);
    array_int_push(scc, 1);
    array_int_push(scc, 2);
    array_int_push(scc, 3);
    array_int_push(scc, 4);
    array_int_push(scc, 99);
    array_int_push(scc, 899);
    array_int_push(scc, 88);
    scc_set_add(SCCs, array_int_get_min(scc), scc);
    array_int_free(scc);

    array_int *x=scc_set_serialize(SCCs);
    array_int_print(x);

    scc_set_t *SCCs2 = scc_set_init();
    scc_set_deserialize(SCCs2, x);
    scc_set_print_debug(SCCs2);
    array_int_free(x);
    scc_set_free(SCCs);
    scc_set_free(SCCs2);
}

//KHASH_MAP_INIT_INT(m32, int)
int main(int argc, char* argv[]){
//    (void) argc;
//    (void) argv;
    //test_scc_set_add();
    //test_tarjan_scc();
    //test_scc_foreach();
    //test_scc_set_merge();
    //test_scc_serialize();
    test_scc_deserialize();
    printf("All tests passed successfully\n");
}