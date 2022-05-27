# Sensor Monitoring System
ECEN 5613: ESD : Spring 2021 : Final Project : Sensor Monitoring System (SMS)

### Project Description
![image](https://user-images.githubusercontent.com/77663015/170604769-d5feff50-4816-462e-9dc3-6e49e92ac8da.png)

In this project, a power supply is designed to provide a source of 3.3V to power on MSP. 
The data Temperature and Humidity data is received from the SI7021, AM2320 sensors over I2C serial communication. This data is subjected to separate processing. As for the first part, the data is sent directly over to the LCD display over SPI protocol whereas for the second part, the temperature and humidity data is appended to the timestamp generated from the onboard RTC and is sent over UART using FTDI converter to the local PC for analysis. The temperature and humidity data are represented graphically using python as well as it is stored in the PC for diagnostic purpose.

### Firmware Design:
![image](https://user-images.githubusercontent.com/77663015/170607115-90396631-7527-4a03-93d2-2343a16da659.png)
  
  The code was architected keeping following features in mind:  
a.	Modular  
b.	Readability  
c.	Sequential in terms of process the data (temperature, humidity) and display it, take control action if any, go to sleep for the time requested and then redo the same process when there is time to wake up.  
d.	Data security by trying to provide only the required information in the header files i.e., there are only few instances where Macros are defined in the header file. Thus, providing abstraction.  
e.	The code is also ensured to be easily reconfigurable by providing custom macros due to which it is not required for one to change the code at each and every line to maximum extent possible. Due to time constraints, every pin may not have been assigned a macro, but efforts were taken to provide the same to maximum extent possible. In case of MSP pin damage for I2C communication, one can shift to another set of alternative pins available by changing the pin assignment.  

### Hardware Design:

In the below picture,  
![image](https://user-images.githubusercontent.com/77663015/170607695-e8804c43-f562-4ff4-950b-78d0c039aaec.png)  
![image](https://user-images.githubusercontent.com/77663015/170608003-70cb925c-b54d-4511-b4d7-1e7d325b8067.png)  


Red 		    ->	Buck converter circuit  
Yellow 		  -> 	LDO circuit  
Blue 		    ->	AM2320 circuit  
Orange 		  -> 	SI7021 circuit  
Green 		  -> 	LCD’s Current Limiting Resistor. The LCD module would have been fitted in the area indicated by green. Since, a damaged LCD was received, and the LCD received from senior was to be returned it was not mounted.  
Dark Blue 	-> 	Place where MSP would have been mounted.  
Red LED 	  -> 	+5V supply  
Green LED 	-> 	+3.35V supply  
  
The board design was planned for placement before soldering.  
  
![image](https://user-images.githubusercontent.com/77663015/170607283-d558ced7-50d3-4ea0-a7b7-8bea5eb2fcb7.png)
  

### Video::
* [Link to Project Video](https://drive.google.com/file/d/1mxA5dylDi5CMdcsoO84ht7NgHJSxN-CX/view?usp=sharing)

### Output

#### Value displayed on LCD 
![image](https://user-images.githubusercontent.com/77663015/170608313-09d67938-1909-4ac0-8aab-b55bf321f0f4.png)


#### Variation in temperature and Humidity values 
![image](https://user-images.githubusercontent.com/77663015/170608242-0f3fe8de-1dd0-40b3-bf28-05f1c22679b6.png)  
The graph was checked to see if it showed an increase (lighter was lit and air was blown near the sensor.) and decrease (after some time) in the temperature and humidity values. The behavior was same as per the expectation.  
### Environment
* IDE: CodeComposer Studio
* Windows 10
* Board/Hardware: MSP432P401R (NXP)

### Author
* Rishab Shah

### References
Firmware - Code references:  
[a]: https://github.com/adafruit/Adafruit_AM2320/blob/master/Adafruit_AM2320.cpp  
[b]: https://github.com/adafruit/Adafruit_Si7021  
[c]: http://rohitg.in/2014/11/09/Nokia5510-lcd-with-MSP430/  
[d]: https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library  
[e]: https://www.oreilly.com/library/view/making-embedded-systems/9781449308889/  
  
Software - Python references:  
[f]: https://pypi.python.org/pypi/pyserial  
[g]: http://embeddedlaboratory.blogspot.com/2016/06/serial-communication-using-python.html  
[h]: https://makersportal.com/blog/2018/2/25/python-datalogger-reading-the-serial-output-from-arduino-to-analyze-data-using-pyserial  
[i]: https://www.w3schools.com/python/python_file_handling.asp  
  
Other Web sources:  
Dew Point calculation: https://iridl.ldeo.columbia.edu/dochelp/QA/Basic/dewpoint.html  

