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

#include <stdio.h>
#include <stdio.h>
#include "bitarray.h"

// kernel uses 10 registers
__global__ void DeleteTrivialSCCs(int* adj_lists, int* adj_list_indexes, int n_vertices, int *bitmask, bool *terminate) {
    int vertex_id = blockDim.x * blockIdx.x + threadIdx.x;
    //If thread is not associated with a vertex or the vertex has already been eliminated then do nothing
    //printf("vertex: %d vertex_id: %d n_vertices: %d\n", vertex_id, vertex_id, n_vertices);
    if(vertex_id >= n_vertices){
        return;
    }

    if(test_bit(bitmask, vertex_id) != 0){ 
        //printf("vertex: %d has already been eliminated\n", vertex_id);
        return;
    }
    int adj_list_start = adj_list_indexes[vertex_id];
    int adj_list_end = adj_list_indexes[vertex_id+1];
    int elim = false;
    //printf("vertex: %d adj_list_start: %d adj_list_end: %d\n", vertex_id, adj_list_start, adj_list_end);
    //If adjacency list is empty or has a self-loop delete the vertex
    if(adj_list_start == adj_list_end){
        elim = true;
    }
    if(((adj_list_start + 1) == adj_list_end) && adj_lists[adj_list_start] == vertex_id){
        elim = true;
    }

    if(elim == true){
        //printf("Sto eliminando1\n");
        (*terminate) = false;
        set_bit(bitmask, vertex_id);
        //printf("vertex: %d eliminated1\n", vertex_id);
        return;
    }

    //If vertex has no incoming edges delete the vertex
    elim = true;
    //printf("adj_list_indexes[n_vertices]%d\n",adj_list_indexes[n_vertices]);
    for(int i = 0; i < adj_list_indexes[n_vertices]; i++){
        if(adj_lists[i] == vertex_id){ //! Manca una condizione
            //printf("vertex: %d found\n", vertex_id);
            elim = false;
            break;
        }
    }

    if(elim == true){
        //printf("vertex: %d eliminated2\n", vertex_id);
        (*terminate) = false;
        set_bit(bitmask, vertex_id);
    }
}