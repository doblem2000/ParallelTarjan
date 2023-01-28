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
#include <iostream>
#include <stdlib.h>
#include <sys/resource.h>
#include "cuda_graph.h"
#include "preprocess.cu"
#include "array.h"
#include "graph.h"
#include "measurement.h"
#include "mpi_logic.h"


#define THREADxBLOCK 1024

using namespace std;

scc_set_t *SCCs;


int main(int argc, char **argv){
    char path_inputfilename[100];
    char output_filename[100];
    int *bitmask, *d_bitmask;
    int *d_adj_lists, *d_adj_list_indexes;
    bool terminate = false;
    bool *d_terminate;

    int rank, size;
    struct rlimit rl;
    getrlimit(RLIMIT_STACK, &rl);
    rl.rlim_cur=128000000;
    setrlimit(RLIMIT_STACK, &rl);
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int num;
    double temp=0.0,time_total_mpi_tarjan = 0.0,time_init = 0.0,time_preprocess=0.0,time_graph_conversion=0.0,time_destroy=0.0;

    if(argc != 3 ){
        printf("Error! Wrong or missing parameters. Please run the program specifing the path of the graph to compute and the name the output file.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    sscanf(argv[1],"%s",path_inputfilename);
    sscanf(argv[2],"%s",output_filename);

    if(size <= 1){
        //Se il numero di processi è 1 allora non posso eseguire il programma in maniera parallela
        printf("This application is meant to be run with at least 2 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if(rank == 0){

        STARTTIME(1);
        SCCs = scc_set_init();

        cuda_graph_t *cuda_graph = cuda_graph_load_from_file(path_inputfilename);
        int n_vertices = cuda_graph->n_vertex;
        int n_bitmask = ((n_vertices-1)/32)+1;
        bitmask = new int[n_bitmask](); //Instantiate an array and initialize it to 0
        //Eseguire il kernel
        int gridsize = ((n_vertices-1)/THREADxBLOCK) + 1; //Numero blocchi su una dimensione della griglia
        dim3 dimGrid(gridsize);
        dim3 dimBlock(THREADxBLOCK);

        cudaMalloc(&d_adj_lists, cuda_graph->adj_lists_len * sizeof(int));
        cudaMemcpy(d_adj_lists, cuda_graph->adj_lists, cuda_graph->adj_lists_len * sizeof(int), cudaMemcpyHostToDevice);
        cudaMalloc(&d_adj_list_indexes, (n_vertices + 1) * sizeof(int));
        cudaMemcpy(d_adj_list_indexes, cuda_graph->adj_list_indexes, (n_vertices + 1) * sizeof(int), cudaMemcpyHostToDevice);

        cudaMalloc(&d_bitmask, n_bitmask * sizeof(int));
        cudaMemset(d_bitmask, 0, n_bitmask * sizeof(int));

        cudaMalloc(&d_terminate, sizeof(bool));
        ENDTIME(1,time_init);
        
        printf("%d,",n_vertices);

        int iter = 0;
        while(!terminate){
            terminate = true;
            cudaMemset(d_terminate, true, sizeof(bool));
            
            STARTTIME(2);
            DeleteTrivialSCCs<<<dimGrid, dimBlock>>>(d_adj_lists, d_adj_list_indexes, n_vertices, d_bitmask, d_terminate);
            cudaDeviceSynchronize();
            ENDTIME(2,temp);
            time_preprocess += temp;
        
            cudaMemcpy(&terminate, d_terminate, sizeof(bool), cudaMemcpyDeviceToHost);
        }
        cudaMemcpy(bitmask, d_bitmask, n_bitmask * sizeof(int), cudaMemcpyDeviceToHost);
        
        //Lanciare tarjan sequenziale
        STARTTIME(3);
        graph_t* graph = cuda_graph_to_graph(cuda_graph, bitmask);
        ENDTIME(3,time_graph_conversion);
        
        STARTTIME(4);
        
        master_work2(rank,size,graph,SCCs,output_filename,time_init);
        
        ENDTIME(4,time_total_mpi_tarjan);

        STARTTIME(5);
        scc_set_save_to_file(SCCs,output_filename);
        cuda_graph_free(cuda_graph);
        cudaFree(d_adj_list_indexes);
        cudaFree(d_adj_lists);
        cudaFree(d_bitmask);
        cudaFree(d_terminate);
        graph_free(graph);
        scc_set_free(SCCs);
        delete[] bitmask;
        
        ENDTIME(5,time_destroy);
        printf("%f,%f,%f,%f,",time_total_mpi_tarjan,time_preprocess,time_graph_conversion,time_destroy);

    }

    if(rank != 0){
        slave_work(rank);
    }

    MPI_Finalize();
    return 0;
}