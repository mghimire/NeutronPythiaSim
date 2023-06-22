#!/bin/bash

# Set the number of parallel instances
num_instances=1000

# Create an array to store the process IDs
pids=()

# Launch parallel instances of the C++ program
for ((i=0; i<num_instances; i++))
do
    # Pass the input parameters to the C++ program as command-line arguments
    ./py_sim -n 10000 -t 0 -f $inputstr output_files/out$i.root &

    # Store the process ID of the launched instance
    pids+=($!)

    # Optionally, you can introduce some delay between launching each instance
    # sleep 1
done

# Wait for all instances to finish
for pid in ${pids[@]}
do
    wait $pid
done

echo "Parallel execution complete."

