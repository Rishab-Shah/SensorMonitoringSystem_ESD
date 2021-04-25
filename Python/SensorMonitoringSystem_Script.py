import serial.tools.list_ports as port_list
ports = list(port_list.comports())
for p in ports: print (p)

import serial
import csv
import io
import numpy                    #Import numpy
import matplotlib.pyplot as plt #import matplotlib library
from drawnow import *

#Can be Downloaded from this Link
#https://pypi.python.org/pypi/pyserial

#Global Variables
sensordata = 0
tempC = []
humidity = []
count = 1           ## row index
filename = "D:\MS_Engg\ESD\Python_code\SMS_Logger.txt"

#Function to Initialize the Serial Port
def init_serial():
    COMNUM = 5                                      #Enter Your COM Port Number Here.
    global sensordata                               #Must be declared in Each Function
    sensordata = serial.Serial()
    sensordata.baudrate = 9600
    sensordata.port = "COM{}".format(COMNUM-1)
    
    #Specify the TimeOut in seconds, so that SerialPort
    #Doesn't hangs
    #sensordata.timeout = 10
    sensordata.open()                               #Opens SerialPort
    plt.figure('Sensor Monitoring System - Rishab Shah - ECEN 5613')
    #print port open or closed
    if sensordata.isOpen():
        print ('Open: ' + sensordata.portstr)
#Function Ends Here


#Function ends here    
def makeFig():                                      #Create a function that makes our desired plot
    plt.ylim([20.0,27.0])                           #Set y min and max values
    plt.title('Live Data Stream')                   #Plot the title
    plt.grid(True)                                  #Turn the grid on
    plt.ylabel('Temp °C')                           #Set ylabels
    plt.plot(tempC, 'ro-', label='Degrees C')       #plot the temperature
    plt.legend(loc='upper left')                    #plot the legend
    plt2=plt.twinx()                                #Create a second y axis
    plt.ylim(20,40)                                 #Set limits of second y axis- adjust to readings you are getting
    plt2.plot(humidity, 'b^-', label='Humidity')    #plot pressure data
    plt2.set_ylabel('Humidity (RH)')                #label second y axis
    plt2.ticklabel_format(useOffset=False)          #Force matplotlib to NOT autoscale y axis
    plt2.legend(loc='upper right')                  #plot the legend

#Call the Serial Initilization Function, Main Program Starts from here
init_serial()

datafile = open(filename,'w')
plt.ion()                                           #Tell matplotlib you want interactive mode to plot live data
cnt=0

#temp = sensordata.write('Type what you want to send, hit enter:\r\n'.encode())
#sensordata.write(temp)                             #Writes to the SerialPort

while True: # While loop that loops forever
    while (sensordata.inWaiting()==0):              #Wait here until there is data
        pass #do nothing
    sensorstring = sensordata.readline()            #read the line of text from the serial port
    #print (''.encode() + sensorstring)
    sensorstring = sensorstring.decode("utf-8").strip()
    backuptext = sensorstring
    p = str(backuptext)
    print(backuptext)
    
    datafile = open(filename,'a')

    datafile.write(p + "\n")                                   
    datafile.close()
            
    dataArray = sensorstring.split('::')
    dataArray = dataArray[1].split(',')             #Split it into an array called dataArray

    #print (dataArray)
    temperature = float( dataArray[0])              #Convert first element to floating number and put in temp
    hum =    float( dataArray[1])                   #Convert second element to floating number and put in P

    tempC.append(temperature)                       #Build our tempC array by appending temp readings
    humidity.append(hum)                            #Building our humidity array by appending P readings
    drawnow(makeFig)                                #Call drawnow to update our live graph
    plt.pause(.000001)                              #Pause Briefly. Important to keep drawnow from crashing
    cnt = cnt+1
    if(cnt > 50):                                     #If you have 50 or more points, delete the first one from the array
       tempC.pop(0)                                 #This allows us to just see the last 50 data points
       humidity.pop(0)
datafile.close()
sensordata.close()
        
#while 1:    
 #   bytes = sensordata.readline()                  #Read from Serial Port
    #print ('You sent:'.encode() + bytes)           #Print What is Read from Port
  #  print (''.encode() + bytes)
#Ctrl+C to Close Python Window

