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
 * @brief call sequential tarjan on graph from file 
 * 
 * the first parameter is the path of the graph whose SSC we want to find with tarjan \n
 * the second parameter is the path of resul file with SCC disovered \n
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "graph.h"
#include "array.h"
#include "args.h"
#include "measurement.h"
#include <sys/resource.h>

scc_set_t *SCCs;

void callback(array_int * scc){
    int scc_id;
    scc_id = array_int_get_min(scc);
    scc_set_add(SCCs,scc_id,scc);
}

int main(int argc, char* argv[]){
    main_parameters_t c;
    struct rlimit rl;
    getrlimit(RLIMIT_STACK, &rl);
    //printf("%d %d\n",rl.rlim_cur,rl.rlim_max);
    rl.rlim_cur=128000000;
    setrlimit(RLIMIT_STACK, &rl);
    //printf("%d %d\n",rl.rlim_cur,rl.rlim_max);
    c = get_input(argc, argv);
    graph_t *graph;

    int num;
    double time_tarjan = 0.0, time_init = 0.0,time_destroy=0.0;

    switch (c.t){
        case INPUT_ERROR:
            printf("incorrent parameters:\ninserire come primo parametro path del grafo ingresso\ninserire come secondo parametro path del file contenente gli SCC trovati\n");
            exit(1);
            break;
        case INPUT_TYPE_FILE:
            {
            STARTTIME(1);
            graph = graph_load_from_file(c.first_param);
            SCCs= scc_set_init();
            //printf("start graph\n");
            //graph_print_debug(graph);
            ENDTIME(1,time_init);

            STARTTIME(2);
            graph_tarjan_foreach(graph, callback);
            ENDTIME(2,time_tarjan);

            num = graph_get_num_vertex(graph);
            STARTTIME(3);
            scc_set_save_to_file(SCCs,c.second_param);
            graph_free(graph);
            scc_set_free(SCCs);
            ENDTIME(3,time_destroy);

            printf("%d,%f,%f,%f,",num,time_init,time_destroy,time_tarjan);
            break;
            }
    }

}

