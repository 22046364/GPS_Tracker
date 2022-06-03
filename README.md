# GPS_Tracker
Adruino Nano code used for GPS tracker and logging device.

Three breakout modules are used in each device. To capture acceleration, an Adafruit ADXL345 - Triple Axis Accelerometer is used. It is interfaced with the Arduino through I2C communication. 

An Adafruit Ultimate GPS Breakout was used to log GPS locations. This breakout includes an antenna. Serial communication is used with this module. 

All of the data is stored to an onboard SD card, through a High Speed SD/Micro SD Card Reader by Robotdyn. This is done through a SPI interface. 

Low correlation in the logged altitude by the GPS model was found, thus altitude data was added with the use an online altitude tool, gpsvisualizer.com, and using the GPS location as input. 

The code allows for 2 inputs and outputs as part of the user interface. Two flip switches serves as input for power and to start recording. LEDs to indicate GPS fix and succesfull active recording is also available
