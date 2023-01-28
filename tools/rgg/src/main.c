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
 * @brief This tool generates a graph starting from a seed.
 * 
 * This tool uses an interger n to generate a graph with 2^n replicas of the seed keeping 
 * all the edges already present in the seed, in addition edges are added between the different seeds 
 * of the final graph following the probability passed. \n
 * The first parameter is the path of seed graph. \n
 * The second parameter is the path of graph generated. \n
 * The thrid parameter is an integer n that indicate the 2^n copy to be created. \n
 * The fourth parameter is the  probability of create an edge between a node of a copy and another and viceversa. \n
 *
 * Es: seed 10 edge and second parameter 1 graph generated 20 edge. \n  
 * Es: seed 10 edge and second parameter 2 graph generated 40 edge. \n  
 * Es: seed 10 edge and second parameter 3 graph generated 80 edge. \n  
 * Es: seed 10 edge and second parameter 4 graph generated 160 edge. \n
 *
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "graph.h"
#include "array.h"
#include "args.h"

int main(int argc, char* argv[]){
    srand(42);
    
    if(argc == 5){
        int n=0;
        int i=0;
        int p=0;
        char path[200];
        char output[200];
    
        sscanf(argv[1],"%s",path);
        sscanf(argv[2],"%s", output);
        sscanf(argv[3],"%d", &n);
        sscanf(argv[4],"%d", &p);
        if(n<0){
            printf("Enter second parameter greater than or equal to 0.\n");
            return 1;
        }

        graph_t * seed, *c;
        seed=graph_load_from_file(path);
        // il seed è in "./../../data/seed.bin"

        for(i=0; i<n; i++){
            c=graph_copy(seed);
            graph_merge(seed, c, p);
            graph_free(c);
        }
        graph_save_to_file(seed, output);

        graph_free(seed);
    }else{
        printf("Insert as first parameter path of the graph seed (graph.bin)\nInsert as second parameter path of the graph output (out. bin)\nInsert as the third parameter the number of multiplications of that seed\n e.g.: seed 10 nodes and second parameter 1 generated graph 20 nodes\n e.g.: seed 10 nodes and second parameter 2 generated graph 40 nodes\n e.g.: seed 10 nodes and second parameter 3 generated graph 80 nodes\n e.g.: seed 10 nodes and second parameter 4 generated graph 160 nodes\n Insert as fourth parameter probability of creating an arc between a node of the starting seed and the new added graph\n");
        return 1;

    }
               
}
