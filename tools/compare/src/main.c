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
 * @brief This tool compare two different SCC discovered file. 
 * 
 * Checks whether all SSCs in the first file are contained in the second file and vice versa. 
 * This is useful to verify the correctness of the parallel algortims by comparing the SSCs 
 * found with those found by sequential Tarjan. \n
 * 
 * The first parameter is the path of the first SCC file to be compered. \n
 * The second parameter is the path of the second SCC file to be compered. \n
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
    
    if(argc == 3){
        char first[200];
        char second[200];
    
        sscanf(argv[1],"%s",first);
        sscanf(argv[2],"%s", second);

        scc_set_t * result1=scc_set_init();
        scc_set_t * result2=scc_set_init();

        result1= scc_set_load_from_file(first);
        result2= scc_set_load_from_file(second);

        if(scc_set_contains(result1, result2) && scc_set_contains(result2, result1)){
            printf("correct output\n");
        }else{
            printf("incorrect output\n");
        }
        scc_set_free(result1);
        scc_set_free(result2);
    }else{
        printf("incorrect param");
    }
               
}
