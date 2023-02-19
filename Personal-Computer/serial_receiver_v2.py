# [NOTE] This program runs on a personal computer not on the Arduino despite some variables refering to the Arduino
# This program must be ran first before the Arduino button to send data is pushed (also have serial monitor closed in Arduino IDE)

import serial

msg_array = []
total_strain_data = []
total_valve_data = []
total_time_data = []
total_setpoint_data = []

connection = serial.Serial('COM4', 57600) # A decently high baudrate is needed so the Arduino doesn't take so long to trasmit data that it leaves large gaps data collection
while True:
    while connection.in_waiting == 0:
        pass
    
    msg = connection.readline().decode().rstrip('\r\n')
    
    if msg != "END" and msg != "OVER":    # Preventing the flags "END" and "OVER" from entering the payload
        #print(f"Gathered Data '{msg}'")
        msg_array.append(msg)
    
    if msg == "END":
        print("Filtering Data...")
        
        # Finding indices of desired data
        strain_data_pointer = msg_array.index('SD') + 1
        valve_data_pointer = msg_array.index('VD') + 1
        time_data_pointer = msg_array.index('TD') + 1
        setpoint_data_pointer = msg_array.index('SP') + 1
        
        # Slicing out desired data from the message bundle
        strain_data = msg_array[strain_data_pointer:valve_data_pointer-1]
        valve_data = msg_array[valve_data_pointer:time_data_pointer-1]
        time_data = msg_array[time_data_pointer:setpoint_data_pointer-1]
        setpoint_data = msg_array[setpoint_data_pointer:]

        print(f"strain_data: {strain_data}")
        print(f"valve_data: {valve_data}")
        print(f"time_data: {time_data}")
        print(f"setpoint_data: {setpoint_data}")
        
        # Accumulating the desired, filtered, data into a single array each
        total_strain_data.extend(strain_data)
        total_valve_data.extend(valve_data)
        total_time_data.extend(time_data)
        total_setpoint_data.extend(setpoint_data)
        
        # Clearing the message bundle for another round of messages
        msg_array = []
        print("Filtered Data")

    if msg == "OVER":
        print("Processing Data...")
        break

# Converts the list of strings representing numbers into actual numeric data type
total_strain_data = list(map(float, total_strain_data))
total_valve_data = list(map(int, total_valve_data))
total_time_data = list(map(int, total_time_data))
total_setpoint_data = list(map(float, total_setpoint_data))

print(f"total_strain_data {total_strain_data}")
print(f"total_valve_data {total_valve_data}")
print(f"total_time_data {total_time_data}")
print(f"total_setpoint_data {total_setpoint_data}")

# Storing the data as a .mat file which is uniquely named based on todays date and military time (copy paste .mat file into MATLAB and load into workspace for analysis)
from scipy.io import savemat
import datetime
data_directory = './Mat_Data/' # [TODO] Generalize this more perhaps with creating the directory if it doesn't exist or perhaps using environmental variables by the user
today = datetime.datetime.now()
filename = f"{today.month}-{today.day}-{today.year}--{today.hour}-{today.minute}-{today.second}"

data = {'TD': total_time_data, 'VD': total_valve_data, 'SD': total_strain_data, 'SP': total_setpoint_data}
with open(data_directory + "data-" + filename + ".mat", 'wb') as file:
    savemat(file, data)