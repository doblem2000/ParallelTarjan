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

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "measurement.h"
#include <sys/resource.h>
#include "mpi_logic.h"

int main(int argc,char* argv[]){
    int rank, size;
    struct rlimit rl;
    getrlimit(RLIMIT_STACK, &rl);
    rl.rlim_cur=128000000;
    setrlimit(RLIMIT_STACK, &rl);
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    char path[100];
    char outputfilename[50];

    if(argc != 3 ){
        printf("Error! Wrong or missing parameters. Please run the program specifing the path of the graph to compute and the name the output file.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }


    sscanf(argv[1],"%s",path);
    sscanf(argv[2],"%s",outputfilename);

    if(size <= 1){
        //Se il numero di processi è 1 allora non posso eseguire il programma in maniera parallela
        printf("This application is meant to be run with at least 2 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    //Convenzioni: il processo con rank 0 è il nodo master
    //I processi con rank diverso da 0 sono nodi slave

    if(rank == 0){
        master_work(rank,size,path,outputfilename);
    }

    if(rank != 0){
        slave_work(rank);
    }

    MPI_Finalize();
    return 0;
}