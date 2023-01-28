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
 * @brief This tool generate graph fully connected, graph fully disconnected or graph bipartite.
 * 
 * The first parameter is the path of graph generated. \n
 * The second parameter is a integer that indicate the number of node of graph. \n
 * The thrid parameter is an integer that indicate 0 for graph fully disconnected, 1 for graph fully connected, 2 for grapg bipartite. \n
 *
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "graph.h"
#include "array.h"
#include "args.h"

void generate_bipartite_graph(long n, char *filename){
    FILE *fp = fopen(filename, "w");
    int tmp[3];
    tmp[0] = (n * 3) + 2;
    tmp[2] = -1;
    fwrite(tmp, sizeof(int), 1, fp);
    fwrite(&n, sizeof(int), 1, fp);
    for(long i=0; i<n; i++){
        tmp[0] = i*2;
        tmp[1] = i*2+1; 
        fwrite(tmp, sizeof(int), 3, fp);
    }
    fclose(fp);
}

int main(int argc, char* argv[]){
    
    if(argc == 4){

        graph_t * t;
        char output[200];
        long n_node;
        int mode;
        
        sscanf(argv[1],"%s",output);
        sscanf(argv[2],"%ld", &n_node);
        sscanf(argv[3],"%d", &mode);

        printf("%s , %ld, %d\n", output,n_node,mode);
        if(mode == 0 || mode == 1){
            t=graph_fully_connected_disconnected(n_node,mode);
            graph_save_to_file(t, output);
            graph_free(t);
        } else if (mode == 2){
            generate_bipartite_graph(n_node, output);
        }
        
        //graph_print_debug(t);
        
        return 0;
    }else{
        printf("inserire come  primo parametro path file contente il grafo di uscita,\nsecondo parametro numero nodi da generare,\nterzo parametro 0 per grafo completamente disconesso, 1 per grafo completamente connesso, 2 per grafo bipartito\n");
        return 1;
    }
    
}