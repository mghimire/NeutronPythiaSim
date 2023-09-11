import numpy as np
import csv

# Open the CSV file
csv_file = 'energy_values.csv'

# Read and store "E" values in a NumPy array
e_values = []
with open(csv_file, 'r') as file:
    csv_reader = csv.DictReader(file)
    for row in csv_reader:
        e_values.append(float(row['E']))

# Convert the list to a NumPy array
e_values_array = np.array(e_values)

neutron_mass 	= 0.9395654133

KE_values_array = e_values_array - neutron_mass

# Initialize array of 4vectors
fourvec_array = np.zeros([len(KE_values_array), 9])

# Set charge to 0
fourvec_array[:,0] = 0

# Set neutron mass
fourvec_array[:,1] = neutron_mass

# Set weight to 1
fourvec_array[:,-1] = 1

# Set coordinates at rock wall
rock_thickness = 1.0
x_at_det = 33.
x_at_rock = x_at_det - rock_thickness
fourvec_array[:,2] = x_at_rock

# Find y and z range at rock wall
y_at_det = 1.6/2
x_rock_to_det_ratio = x_at_rock/x_at_det
y_at_rock = y_at_det*x_rock_to_det_ratio
z_at_det = 1.2/2
z_at_rock = z_at_det*x_rock_to_det_ratio

# Randomly distribute y and z values within range of detector and fill out rest of array
for i in range(0,len(KE_values_array)):
    fourvec_array[i,3] 	= np.random.uniform(-1*y_at_rock, y_at_rock)
    fourvec_array[i,4] 	= np.random.uniform(-1*z_at_rock, z_at_rock)
    path_length 	= np.sqrt(fourvec_array[i,2]**2+fourvec_array[i,3]**2+fourvec_array[i,4]**2)
    KE 			= KE_values_array[i]
    fourvec_array[i,5] 	= KE*fourvec_array[i,2]/path_length
    fourvec_array[i,6]	= KE*fourvec_array[i,3]/path_length
    fourvec_array[i,7]  = KE*fourvec_array[i,4]/path_length

N = len(KE_values_array)  # Number of rows
num_columns = 9

# Define the output CSV file name
output_file = '4vecs_9dim.csv'

# Save the numpy array as a CSV file
np.savetxt(output_file, fourvec_array, delimiter=',', header=','.join([f'Column_{i}' for i in range(num_columns)]), comments='')
print(f'Array saved to {output_file}')



