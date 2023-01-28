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
#include <assert.h>
#include "graph.h"
#include "array.h"
#include "args.h"

int main(int argc, char* argv[]){
    if(argc <= 1){
        graph_t * t, *c;
        t=graph_random(10, 2, 1);
        printf("\n\ngrafo t:\n");
        graph_print_debug(t);
        c=graph_copy(t);
        printf("\n\ngrafo c:\n");
        graph_print_debug(c);

        graph_merge(t, c, 0.2);
        printf("\n\nnuovo grafo t:\n");
        graph_print_debug(t);

        printf("\n\ngrafo c:\n");
        graph_print_debug(c);

        graph_free(t);
        graph_free(c);
    } else if(argc == 2){
        int n=0;
        int i=0;
        sscanf(argv[1],"%d", &n);
        printf("creo %d grado di partenza\n", n);

        graph_t * seed, *c;
        seed=graph_random(10, 2, 1);
        printf("\n\ngrafo t:\n");
        graph_print_debug(seed);

        for(i=0; i<n; i++){
            c=graph_copy(seed);
            graph_merge(seed, c, 0.2);
            graph_free(c);
        }
        printf("\n\nnuovo grafo t:\n");
        graph_print_debug(seed);

        graph_free(seed);
        

    }else{
        printf("cosa cazzo hai messo in ingresso cornuto\n");
    }
    
}

///genero seed e metto su file
///leggo seed da file e genero 