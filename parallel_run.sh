#!/bin/bash

# Set the number of parallel instances
num_instances=4

# Set the total number of desired events

num_events=10000000

# Set the total number of events per simulation

num_per_sim=10000

# Find the total number of simulations needed

num_sims=$((($num_events) / $num_per_sim))

# Set the batch size
batch_size=10

# Set batch offset to save separate batches of data
batch_offset=4000

# Calculate the total number of batches

num_batches=$((($num_sims + $batch_size - 1) / $batch_size))

# Create an array to store the process IDs
pids=()

for ((batch=0; batch<num_batches; batch++))
do
    start_index=$(((batch * batch_size)+$batch_offset))
    end_index=$((start_index + batch_size))
    
    # Get the subset of input parameters for the current batch
    batch_inputs=($(seq $start_index $end_index))
    
    for ((i=0; i<${#batch_inputs[@]}; i++))
    do
        # Pass the input parameter to the C++ program as a command-line argument
	./py_sim -n $num_per_sim -t 0 -f output_files/out${batch_inputs[$i]}.root > py_logs/log${batch_inputs[$i]}.txt &
        
        # Store the process ID of the launched instance
        pids+=($!)
    done
    
    # Wait for all instances of the current batch to finish
    for pid in ${pids[@]}
    do
        wait $pid
    done
    
    # Clear the process ID array for the next batch
    pids=()
    
    echo "Batch $((batch + 1)) of $num_batches complete."
done

echo "Parallel execution complete."
