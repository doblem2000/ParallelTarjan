#!/bin/bash
TIME_STAMP=$(date +%s)
#number of measurement for each graph file for any compiler optimization 
NMEASURES=25 #200 anno scorso

#input file graph
ARRAY_RC=(fully-disconnected-1000000 fully-connected-12500 random-150000 random-250000 random-500000 tile-52000 tile-205000 tile-410000 tile-820000)
#ARRAY_RC=(random-1000 random-50000)
#ARRAY_RC=(matrix_11k)

#number mpi process
ARRAY_THS=(2 4 8)
ARRAY_CUDA_THS=(1 2 3 4)
COSTANT_CUDA_THS=4
#TIMEFORMAT='%3U;%3E;%3S;%P'
#compiler optimization
ARRAY_OPT=(0 1 2 3)

trap "exit" INT

# $1: nome del programma

#SCRIPTPATH=$( cd -- "$(dirname "$0")"  >/dev/null 2>&1 ; pwd -P )
SCRIPTPATH=$( cd -- "$(dirname "$0")" ; pwd -P )
if [[ $1 == "sequential" ]]; then
	for input_graph in "${ARRAY_RC[@]}"; do
		for opt in "${ARRAY_OPT[@]}"; do
			ths_str=$(printf "%02d" $ths)
			
			OUT_FILE=$SCRIPTPATH/measure/$1/graph_type-$input_graph-O$opt/$1-graph_type-$input_graph-O$opt.csv
		
			mkdir -p $(dirname $OUT_FILE) #Se non esiste la cartella di OUTFILE viene creata
			
			echo $(basename $OUT_FILE)
			echo "verteces,init,finalize,tarjan,user,system,elapsed,pCPU" >$OUT_FILE
			
			for ((i = 0 ; i < $NMEASURES; i++)); do
				{ /usr/bin/time -f "%U,%S,%e,%P" ../bin/$1_O$opt.out ../data/$input_graph.bin ../data/$1_output_$input_graph.bin; } 2>&1 | sed -e 's/%/;/g'>> $OUT_FILE
				printf "\r> %d/%d %3.1d%% " $(expr $i + 1) $NMEASURES $(expr \( \( $i + 1 \) \* 100 \) / $NMEASURES)
				printf "#%.0s" $(seq -s " " 1 $(expr \( $i \* 40 \) / $NMEASURES))
			done
			printf "\n"
		done
	done
elif [[ $1 == "cuda" ]]; then
	for input_graph in "${ARRAY_RC[@]}"; do
		for ths in "${ARRAY_CUDA_THS[@]}"; do
			for opt in "${ARRAY_OPT[@]}"; do
				
				#IFS='-' array=($input_graph)
				#nodes=${array[-1]}
				nodes=$(echo $input_graph | rev | cut -d'-' -f1 | rev )
				echo $input_graph
				echo $nodes
				thread=$(($ths*$nodes/$COSTANT_CUDA_THS))
				echo $thread

				ths_str=$(printf "%02d" $thread)
				OUT_FILE=$SCRIPTPATH/measure/$1/graph_type-$input_graph-O$opt/$1-graph_type-$input_graph-O$opt-NTH-$ths_str.csv
				echo $OUT_FILE
				mkdir -p $(dirname $OUT_FILE) #Se non esiste la cartella di OUTFILE viene creata
				
				echo $(basename $OUT_FILE)
				echo "verteces,init,finalize,preprocess,conversion,tarjan,user,system,elapsed,pCPU" >$OUT_FILE
				
				for ((i = 0 ; i < $NMEASURES; i++)); do
					{ /usr/bin/time -f "%U,%S,%e,%P" ../bin/$1_O$opt.out ../data/$input_graph.bin ../data/$1_output_$input_graph.bin $thread; } 2>&1 | sed -e 's/%/;/g' >> $OUT_FILE
					printf "\r> %d/%d %3.1d%% " $(expr $i + 1) $NMEASURES $(expr \( \( $i + 1 \) \* 100 \) / $NMEASURES)
					printf "#%.0s" $(seq -s " " 1 $(expr \( $i \* 40 \) / $NMEASURES))
				done
				printf "\n"
			done
		done
	done
elif [[ $1 == "cuda_texture" ]]; then
	for input_graph in "${ARRAY_RC[@]}"; do
		for ths in "${ARRAY_CUDA_THS[@]}"; do
			for opt in "${ARRAY_OPT[@]}"; do
				
				#IFS='-' array=($input_graph)
				#nodes=${array[-1]}
				nodes=$(echo $input_graph | rev | cut -d'-' -f1 | rev )
				echo $input_graph
				echo $nodes
				thread=$(($ths*$nodes/$COSTANT_CUDA_THS))
				echo $thread

				ths_str=$(printf "%02d" $thread)
				OUT_FILE=$SCRIPTPATH/measure/$1/graph_type-$input_graph-O$opt/$1-graph_type-$input_graph-O$opt-NTH-$ths_str.csv
				echo $OUT_FILE
				mkdir -p $(dirname $OUT_FILE) #Se non esiste la cartella di OUTFILE viene creata
				
				echo $(basename $OUT_FILE)
				echo "verteces,init,finalize,preprocess,conversion,tarjan,user,system,elapsed,pCPU" >$OUT_FILE
				
				for ((i = 0 ; i < $NMEASURES; i++)); do
					{ /usr/bin/time -f "%U,%S,%e,%P" ../bin/$1_O$opt.out ../data/$input_graph.bin ../data/$1_output_$input_graph.bin $thread; } 2>&1 | sed -e 's/%/;/g' >> $OUT_FILE
					printf "\r> %d/%d %3.1d%% " $(expr $i + 1) $NMEASURES $(expr \( \( $i + 1 \) \* 100 \) / $NMEASURES)
					printf "#%.0s" $(seq -s " " 1 $(expr \( $i \* 40 \) / $NMEASURES))
				done
				printf "\n"
			done
		done
	done
elif [[ $1 == "mpi_cuda" ]]; then
	for input_graph in "${ARRAY_RC[@]}"; do
		for opt in "${ARRAY_OPT[@]}"; do
			ths_str=$(printf "%02d" $ths)
			
			OUT_FILE=$SCRIPTPATH/measure/$1/graph_type-$input_graph-O$opt/$1-graph_type-$input_graph-O$opt.csv
		
			mkdir -p $(dirname $OUT_FILE) #Se non esiste la cartella di OUTFILE viene creata
			
			echo $(basename $OUT_FILE)

			echo "Nvert,NvertAfterCuda,init,mpi_tarjan,split,merge,total_only_mpi,preprocess,conversion,finalize,user,system,elapsed,pCPU" >$OUT_FILE

			for ((i = 0 ; i < $NMEASURES; i++)); do
				{ /usr/bin/time -f "%U,%S,%e,%P" ../bin/$1_O$opt.out ../data/$input_graph.bin ../data/$1_output_$input_graph.bin; } 2>&1 | sed -e 's/%/;/g' >> $OUT_FILE
				printf "\r> %d/%d %3.1d%% " $(expr $i + 1) $NMEASURES $(expr \( \( $i + 1 \) \* 100 \) / $NMEASURES)
				printf "#%.0s" $(seq -s " " 1 $(expr \( $i \* 40 \) / $NMEASURES))
			done
			printf "\n"
		done
	done
elif [[ $1 == "sequential_pre" ]]; then
	for input_graph in "${ARRAY_RC[@]}"; do
		for opt in "${ARRAY_OPT[@]}"; do
			ths_str=$(printf "%02d" $ths)

			OUT_FILE=$SCRIPTPATH/measure/$1/graph_type-$input_graph-O$opt/$1-graph_type-$input_graph-O$opt.csv
		
			mkdir -p $(dirname $OUT_FILE) #Se non esiste la cartella di OUTFILE viene creata
			
			echo $(basename $OUT_FILE)
			echo "verteces,verteces_after,init,destroy,tarjan,preprocess_time,user,system,elapsed,pCPU" >$OUT_FILE

			for ((i = 0 ; i < $NMEASURES; i++)); do
				{ /usr/bin/time -f "%U,%S,%e,%P" ../bin/$1_O$opt.out ../data/$input_graph.bin ../data/$1_output_$input_graph.bin; } 2>&1 | sed -e 's/%/;/g'>> $OUT_FILE
				printf "\r> %d/%d %3.1d%% " $(expr $i + 1) $NMEASURES $(expr \( \( $i + 1 \) \* 100 \) / $NMEASURES)
				printf "#%.0s" $(seq -s " " 1 $(expr \( $i \* 40 \) / $NMEASURES))
			done
			printf "\n"
		done
	done
else
	for input_graph in "${ARRAY_RC[@]}"; do
		for ths in "${ARRAY_THS[@]}"; do
			for opt in "${ARRAY_OPT[@]}"; do
				ths_str=$(printf "%02d" $ths)
				
				OUT_FILE=$SCRIPTPATH/measure/$1/graph_type-$input_graph-O$opt/$1-graph_type-$input_graph-O$opt-NTH-$ths_str.csv

				mkdir -p $(dirname $OUT_FILE) #Se non esiste la cartella di OUTFILE viene creata
				
				echo $(basename $OUT_FILE)
				echo "Nvert,init,tarjan,split,merge,user,system,elapsed,pCPU" >$OUT_FILE
				
				for ((i = 0 ; i < $NMEASURES; i++)); do
						
					if [ $1 == "mpi" ]; then
						#2>/dev/null;
						{ /usr/bin/time -f "%U,%S,%e,%P" mpiexec -np $ths ../bin/$1_O$opt.out ../data/$input_graph.bin ../data/$1_output_$input_graph.bin; } 2>&1 | sed -e 's/%/;/g' >> $OUT_FILE
						printf "\r> %d/%d %3.1d%% " $(expr $i + 1) $NMEASURES $(expr \( \( $i + 1 \) \* 100 \) / $NMEASURES)
						printf "#%.0s" $(seq -s " " 1 $(expr \( $i \* 40 \) / $NMEASURES))
					else
						printf "Not implemented"
					fi
				done
				printf "\n"
			done
		done
	done
fi