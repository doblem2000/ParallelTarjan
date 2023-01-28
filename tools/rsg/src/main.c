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
 * @file main.c
 * @brief This tool generate a random graph with max_n_node node and each node have mean number of edge with a variance_edge, the number of nodes follows the Bernoulli distribution.
 * 
 * The first parameter is the path of graph generated. \n
 * The second parameter is a integer that indicate the number of node of graph. \n
 * The thrid parameter is an integer that indicate the the mean of edge for each node. \n
 * The fourth parameter is the variance of number of edge for each node. \n
 *
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "graph.h"
#include "array.h"
#include "args.h"
#include <time.h>

int main(int argc, char* argv[]){
    srand(time(NULL));
    
    if(argc == 5){

        char output[200];
        int n_node;
        int mean_edges;
        double variance_edges;
        
        sscanf(argv[1],"%s",output);
        sscanf(argv[2],"%d", &n_node);
        sscanf(argv[3],"%d", &mean_edges);
        sscanf(argv[4],"%lf", &variance_edges);

        printf("%s , %d, %d, %f\n", output,n_node,mean_edges,variance_edges);

        graph_t * t;
        t=graph_random(n_node, mean_edges, variance_edges);
        //graph_random(1000, 2, 1);
        graph_save_to_file(t, output);
        graph_free(t);

    }else{
        printf("inserire come  primo parametro path file contente il grafo di uscita,\nsecondo parametro numero nodi da generare,\nterzo parametro media archi da generare per ogni nodo,\nquarto parametro varianza archi da generare per ogni nodo.\n");
        return 1;

    }
    
}