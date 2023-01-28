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
 * @file mpi_logic.c
 * @brief This file implements a version of a parallelization of Tarjan's algorithm. 
 */
#include <mpi.h>
#include "mpi_logic.h"
#include "measurement.h"


double time_split_graph = 0.0,time_merge_graph = 0.0;

/**
 * @brief This is a callback function. It is called every time the Tarjan'algorithm, run by a slave process on a portion of the graph, finds an scc. 
 * \n The slave process sends the found scc to the master node along with its size.        
 * 
 * @param scc It is the scc discovered from the Tarjan's algorithm.
 */
void callback(array_int * scc){
    int scc_size = array_int_length(scc);

    MPI_Send(&scc_size,1,MPI_INT,MASTER,MPI_TAG_SIZE,MPI_COMM_WORLD);
    MPI_Send(array_int_get_ptr(scc),array_int_length(scc),MPI_INT,MASTER,MPI_TAG_DATA,MPI_COMM_WORLD);
    
}



/**
 * The function takes as input the graph, the size of the chunk, the number of slave processes and the data structure where to save all the scc found.
 * \n The function sends a chunk of the graph to each slave node. Then, it waits for the slave nodes to find the scc by applying Tarjan's algorithm on their chunk of the graph. As soon as a slave node finishes execution, the master node assigns it another chunk of the graph. 
 * \n The iterations terminate as soon as the whole graph for the fixed chunk size has been completed by the slave nodes.
 * 
 * @param graph graph on which compute all the scc.
 * @param N represents the chunk size.
 * @param n_slaves number of slave precesses.
 * @param SCCs data structure where to save all the scc found.
*/
void master_schedule(graph_t* graph,int N,int n_slaves,scc_set_t *SCCs){
    //codice del master
    MPI_Status status_send_size,status_send_data;
    double partial_time_split = 0.0, partial_time_merge = 0.0;
    MPI_Status status_data,status_size;
    MPI_Request request_data,request_size;
    
    khint_t pos = 0;

    array_int* serialized_graph_chunk;
    
    array_int* scc;
    
    int scc_id;
    int still_working = 0; //rappresenta il numero di precessi slave che stanno lavorando.
    int msg_size;
    int scc_size;
    int done,finished = 0; //Inizializzo finish a 0


    for(int i = 0; i<n_slaves; i++){
        STARTTIME(4);
        serialized_graph_chunk = graph_serialize(graph, N , &pos); //Serializes at most n nodes of the graph starting from the specified bucket. Array contains [n_vertex id_1 adj_1 -1 id_2 adj2 -1] etc...
        ENDTIME(4,partial_time_split);
        time_split_graph += partial_time_split;
        msg_size = array_int_length(serialized_graph_chunk);

        if(array_int_get(serialized_graph_chunk, 1) != 0){ //Se è 0 vuol dire che non ho più chunk da asseganare agli slave perché la struttura dati ha esaurito vertici
            still_working++;
        }else{
            finished = 1;
            break;
        }
        //comunicazione dell'array agli slave
        MPI_Send(&msg_size,1,MPI_INT, i+1,MPI_TAG_SIZE,MPI_COMM_WORLD);
        MPI_Send(array_int_get_ptr(serialized_graph_chunk),msg_size,MPI_INT,i+1,MPI_TAG_DATA,MPI_COMM_WORLD);

    }
    
    while(!finished || still_working != 0){
    
        //I messaggi degli slave hanno due TAG. Il primo tag specifica che il contenuto del messaggio sono dati ed il secondo è per comunicare la size del messaggio. 
        
        //I nodi slave inviano gli scc che hanno trovato. Il nodo master deve poter riceverli.
        //Il primo messaggio ricevuto dal nodo master è sempre la size di un messaggio. 
        //Se il nodo master riceve un messaggio di size 0 allora questo significa che il processo slave ha terminato la sua esecuzione e il nodo master deve decrementare la variabile still_working.
        //Se un processo slave ha iniviato un scc al nodo master, gli viene assegnato un nuovo chunk del grafo su cui lavorare.
        //Il loop si interrompe se tutti gli slave hanno terminato e tutti i chunk del grafo sono stati elaborati.    
        MPI_Recv(&scc_size,1, MPI_INT,MPI_ANY_SOURCE,MPI_TAG_SIZE,MPI_COMM_WORLD,&status_size);
        
        if(scc_size == 0 && !finished){
            STARTTIME(5);
            serialized_graph_chunk = graph_serialize(graph, N , &pos); //Serializes at most n nodes of the graph starting from the specified bucket. Array contains [n_vertex id_1 adj_1 -1 id_2 adj2 -1] etc...
            ENDTIME(5,partial_time_split);
            time_split_graph += partial_time_split;
            msg_size = array_int_length(serialized_graph_chunk);
            if(array_int_get(serialized_graph_chunk, 1) == 0){ //Se la msg_size è 0 vuol dire che non ho più chunk da asseganare agli slave perché la struttura dati ha esaurito vertici
                finished = 1;
                still_working--;
            }else{ 
                //Assegno allo slave che ha finito un altro chunck 
                MPI_Send(&msg_size,1,MPI_INT,status_size.MPI_SOURCE,MPI_TAG_SIZE,MPI_COMM_WORLD);        
                MPI_Send(array_int_get_ptr(serialized_graph_chunk),msg_size,MPI_INT,status_size.MPI_SOURCE,MPI_TAG_DATA,MPI_COMM_WORLD);
            }
        }else if(scc_size == 0){
            still_working--;
        }else{
            scc = array_int_init(scc_size);
            array_int_resize(scc, scc_size);
            MPI_Recv(array_int_get_ptr(scc),scc_size,MPI_INT,status_size.MPI_SOURCE,MPI_TAG_DATA,MPI_COMM_WORLD,&status_data);
            
            STARTTIME(6);
            //Devo aggiungere l'scc che ho trovato in una NUOVA struttura dati. Così da poter ricordare quali sono gli scc trovati
            scc_id = array_int_get_min(scc);
            graph_merge_vertices(graph,scc_id,scc);
            scc_set_add(SCCs,scc_id,scc);

            array_int_free(scc);
            ENDTIME(6,partial_time_merge);
            time_merge_graph += partial_time_merge;
        }
    }

    

}



/**
 * @brief The master node calls this function. 
 * \n The function takes as input the name of a binary file that contains a graph represented by an adjacency map.
 * \n The master_work function takes care of extracting the contents of the binary file and converting it into a graph, then calls the master_work2() function to execute the MPI algorithm. 
 * 
 * @param rank id of the process within the communicator.
 * @param size size of the communicator.
 * @param filename name of the file that contains a graph represented by an adjacency map.
 * @param outputfilename name of the output binary file that will contain all the scc found.
 */
void master_work(int rank,int size,char* filename,char* outputfilename){
    graph_t* graph;
    double time_init;

    STARTTIME(1);
    scc_set_t *SCCs = scc_set_init(); //Set di SCC noti
    graph = graph_load_from_file(filename);
    
    if(graph == NULL){
        printf("[MASTER] Path not found\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    ENDTIME(1,time_init);

    master_work2(rank, size, graph, SCCs, outputfilename, time_init);
    
    graph_free(graph);
    scc_set_free(SCCs);
}


/**
 * @brief This function is called by the master node.
 * \n The function takes as input the graph and an empty set, which will be filled with the scc found by Tarjan's algorithm.
 * \n The function divides the graph into chunks of fixed size and will delegate the work to be done on the chunks to the master_schedule() function.
 * \n In addition, the function is responsible of sending the termination message to all slave processes. This happens when all the work is done.
 * 
 * @param rank id of the process within the communicator.
 * @param size size of the communicator.
 * @param graph graph that will be computed in order to find its sccs. 
 * @param SCCs empty set which will be filled with the scc found by Tarjan's algorithm.
 * @param outputfilename name of the output binary file that will contain all the scc found.
 * @param time_init initialization time.
 */
void master_work2(int rank,int size,graph_t* graph,scc_set_t * SCCs,char* outputfilename, double time_init){
    int v_graph,num_vertex;
    double time_mpi_tarjan;

    STARTTIME(2);
    int i;
    int dim_chunk = graph_get_num_vertex(graph)/(size - 1);
    
    if (dim_chunk > MAX_DIM_CHUNK) {
        dim_chunk = MAX_DIM_CHUNK;
    }
    for(i = dim_chunk; i<=graph_get_num_vertex(graph); i *= 2){
        master_schedule(graph,i,size-1,SCCs);
    }
    if(i>graph_get_num_vertex(graph)){ //Ensure last loop executes on the enterity of the graph.
        i=graph_get_num_vertex(graph);
        master_schedule(graph,i,size-1,SCCs);
    }
    ENDTIME(2,time_mpi_tarjan);
    num_vertex = graph_get_num_vertex(graph);
    printf("%d,%f,%f,%f,%f,",num_vertex,time_init,time_mpi_tarjan,time_split_graph,time_merge_graph);

    scc_set_save_to_file(SCCs,outputfilename);

    //Stop the slaves
    int abort_message = 0;
    //printf("[MASTER] Sending abort messages\n");
    for(i = 1; i < size; i++){
      MPI_Send(&abort_message,1,MPI_INT,i,MPI_TAG_SIZE,MPI_COMM_WORLD);
    }
}

/**
 * @brief This function is called by the slave nodes.
 * The function receives messages containing the portion of the graph on which the slave node must find the scc through Tarjan's algorithm.
 * \n The function ends when a master node sends a special termination message. The termination message is a message specifying that the size of the next message is 0.
 * 
 * @param rank id of the process within the communicator.
 */
void slave_work(int rank){
    graph_t* subgraph;
    array_int* array_scc; 
    int msg_size;
    int scc_size;
    int master = 0;
    int done = 1;
    array_int *buff;
    //int* array;
    MPI_Request slave_request;
    MPI_Status slave_status;


    while(1){
        //Aspetto di ricevere l'array da deserialize
        MPI_Recv(&msg_size,1,MPI_INT,0,MPI_TAG_SIZE,MPI_COMM_WORLD,&slave_status);
        if(msg_size == 0){
          return;
        }
        
        buff = array_int_init(msg_size);
        array_int_resize(buff, msg_size);
        subgraph = graph_init();
        MPI_Recv(array_int_get_ptr(buff),msg_size,MPI_INT,0, MPI_TAG_DATA,MPI_COMM_WORLD,&slave_status);

        //deserializzo il grafo inviato dal master
        graph_deserialize(subgraph, buff);
        array_int_free(buff);
        //Compute scc for the given subgraph

        //Invio array_scc al master
        graph_tarjan_foreach(subgraph,&callback);

        scc_size=0;
        //Comunico al master che il nodo slave ha terminato
        MPI_Send(&scc_size,1,MPI_INT,master,MPI_TAG_SIZE,MPI_COMM_WORLD);
        //printf("[SLAVE] rank: %d ended\n", rank);
        graph_free(subgraph);
    }
}


