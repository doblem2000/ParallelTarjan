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
#include <string.h>
#include <assert.h>
#include "graph.h"
#include "array.h"
#include "cuda_graph.h"
#include "bitarray.h"
int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Invalid parameters.\nUsage: %s [graph]\n", argv[0]);
        exit(1);
    }
    cuda_graph_t *G;
    G = cuda_graph_load_from_file(argv[1]);
    cuda_graph_print_debug(G);
    int *mask = (int *) malloc( sizeof(int) * ( ( (G->n_vertex - 1) / 8 ) + 1 ) );
    memset(mask, 0, sizeof(int) * ( ( (G->n_vertex - 1) / 8 ) + 1 ));
    graph_t *G2 = cuda_graph_to_graph(G, mask);
    graph_print_debug(G2);
    cuda_graph_free(G);
    graph_free(G2);
    free(mask);
    return 0;
}