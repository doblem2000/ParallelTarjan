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
#include <iostream>
#include <stdlib.h>
#include "cuda_graph.h"
#include "preprocess.cu"
#include "array.h"
#include "graph.h"
#include "measurement.h"

#define THREADxBLOCK 512

using namespace std;

scc_set_t *SCCs;

void callback(array_int * scc){
    int scc_id;
    scc_id = array_int_get_min(scc);
    scc_set_add(SCCs,scc_id,scc);
}

int main(int argc, char **argv){
    char path_inputfilename[100];
    char output_filename[100];
    //for mark cancelled node by cuda
    int *bitmask, *d_bitmask;
    //rappresentazione grafi
    int *d_adj_lists, *d_adj_list_indexes;
    //host
    bool terminate = false;
    //device
    bool *d_terminate;

    int num, thread_number;
    double temp=0.0,time_tarjan = 0.0,time_init = 0.0,time_preprocess=0.0,time_graph_conversion=0.0,time_destroy=0.0;

    if(argc < 3 || argc > 4){
        printf("Error! Wrong or missing parameters. Please run the program specifing the path of the graph to compute and the name the output file.\n");
        exit(1);
    }
    sscanf(argv[1],"%s",path_inputfilename);
    sscanf(argv[2],"%s",output_filename);

    STARTTIME(1);
    SCCs = scc_set_init();

    cuda_graph_t *cuda_graph = cuda_graph_load_from_file(path_inputfilename);
    int n_vertices = cuda_graph->n_vertex;
    int n_bitmask = ((n_vertices-1)/32)+1;
    bitmask = new int[n_bitmask](); //Instantiate an array and initialize it to 0
    //Choose thread number
    if(argc == 3){ //No thread number provided -> thread number equal the number of vertices
      thread_number = n_vertices;
    } else { //Thread number provided as argv[3]
      sscanf(argv[3],"%d",&thread_number);
      if(thread_number < 0){ //Negative thread numbers have a special meaning: -n -> n/4 * n_vertices
        thread_number = ((-thread_number)/4.0) * n_vertices;
      }
    }

    //Eseguire il kernel
    int gridsize = ((thread_number-1)/THREADxBLOCK) + 1; //Numero blocchi su una dimensione della griglia
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
    
    int iter = 0;
    while(!terminate){
        //printf di C++
        //cout << "Called " << iter++ << " times" <<endl;
        terminate = true;
        cudaMemset(d_terminate, true, sizeof(bool));
        //printf("start del kernel\n");
        
        STARTTIME(2);
        DeleteTrivialSCCs<<<dimGrid, dimBlock>>>(thread_number, d_adj_lists, d_adj_list_indexes, n_vertices, d_bitmask, d_terminate);
        cudaDeviceSynchronize();
        ENDTIME(2,temp);
        time_preprocess += temp;
    
        //cudaMemcpy(bitmask, d_bitmask, n_bitmask * sizeof(int), cudaMemcpyDeviceToHost);
        /*for(int i = 0; i < n_bitmask; i++){
            printf("%x ", bitmask[i]);
        }
        printf("\n");*/
        //device to host
        cudaMemcpy(&terminate, d_terminate, sizeof(bool), cudaMemcpyDeviceToHost);
    }
    cudaMemcpy(bitmask, d_bitmask, n_bitmask * sizeof(int), cudaMemcpyDeviceToHost);
    
    //printf("Bitmask: %x\n", bitmask[0]);
    //First
    //00000000.00000000.00000000.00011000 <-- 31-0
    //00000000.00000000.00000000.00010001 <-- 63-32
    //00000000.00000000.00000000.00000101 <-- 95-64
    //00000000.00000000.00000000.00000101 <-- 127-96
    //Second
    //00000000.00000000.00100001.00011000 <-- 31-0
    //00000000.00000000.00000100.00010011 <-- 63-32
    //00000000.00000000.00000000.00010111 <-- 95-64
    //00000000.00000000.00000000.00000111 <-- 127-96
    //Third
    //00000000.10000000.00100011.00011000 <-- 31-0
    //00000000.00001000.00000100.00010111 <-- 63-32
    //00000000.00000000.00000000.10011111 <-- 95-64
    //00000000.00000000.00000000.00000111 <-- 127-96

    
    STARTTIME(3);
    graph_t* graph = cuda_graph_to_graph(cuda_graph, bitmask);
    ENDTIME(3,time_graph_conversion);

    
    //graph_print_debug(graph);
    
    //printf("start graph\n");
    //graph_print_debug(graph);
    //Lanciare tarjan sequenziale
    STARTTIME(4);
    graph_tarjan_foreach(graph, callback);
    ENDTIME(4,time_tarjan);

    num = graph_get_num_vertex(graph);
    
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
    printf("%d,%f,%f,%f,%f,%f,",num,time_init,time_destroy,time_preprocess,time_graph_conversion,time_tarjan);

    
    return 0;
}