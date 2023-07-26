from os import listdir, system

logdir = "py_logs"

lognames = listdir(logdir)

N_neutron_gen = 0
N_neutron_log = 0

for logname in lognames:
	with open(logdir + "/" + logname) as f:
		lines = f.readlines()
		N_neutron_gen = N_neutron_gen + int(lines[-3].split(" ")[-1])
		N_neutron_log = N_neutron_log + int(lines[-2].split(" ")[1])

print("Total number of neutrons generated from log files is: "+str(N_neutron_gen))
print("Total number of neutrons saved from log files is: "+str(N_neutron_log))
