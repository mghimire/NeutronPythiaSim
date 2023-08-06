# NeutronPythiaSim
Pythia6 simulation of neutrons created at the CMS IP and detectable by milliQan

[py_sim.cc](py_sim.cc) uses Pythia6 to generate .root files of events containing the particle ID (2112), energies, pT, eta, pTHat, event weight, and event number of neutrons generated at 13.6 TeV pp collisions.
[environ.sh](environ.sh) needs to be sourced to ensure that the root and Pythia8 macros are running properly.

Run ```make all``` to compile [py_sim.cc](py_sim.cc) and automatically source [environ.sh](environ.sh).

[parallel_run.sh](parallel_run.sh) is a macro that runs the Pythia6 simulation in batches. It can be edited to change the number of batches and runs per batch.
[CountEventsFromLogs.py](CountEventsFromLogs.py) is a program that outputs the number of neutrons generated from Pythia8 log files.
