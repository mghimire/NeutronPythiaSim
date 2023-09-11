#!/bin/bash

# Create the output directory if it doesn't exist
mkdir -p out_pt_cut_analysis
mkdir -p py_logs

# Loop through pt_cuts from 1 to 20
for ((i=1; i<=20; i++))
do
    # Set the pt_cut value
    pt_cut=$i

    # Run the C++ program with the specified parameters
    ./py_sim -n 10000 -p ${pt_cut} -f out_pt_cut_analysis/out_pt_cut_${pt_cut}.root > py_logs/log_pt_cut_${pt_cut}.txt

    # Print a message indicating the completion of the run
    echo "Finished running with pt_cut=${pt_cut}"
done
