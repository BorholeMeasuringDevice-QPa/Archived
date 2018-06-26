from picamera import PiCamera
from time import sleep
from time import time
from time import asctime
from time import localtime
from RPi import GPIO
import smbus

clk = 22
dt = 23
led = 21
buton = 20
butof = 16

GPIO.setmode(GPIO.BCM)   
GPIO.setup(clk, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(dt, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(buton, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(butof, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(led, GPIO.OUT)

ledState = GPIO.output (led, GPIO.LOW)
counter = 0
clkLastState = GPIO.input(clk)
start = 0

while start == 0:
    d =GPIO.input(buton)
    if d == 0:
        start = 1
    else:
        start = 0
    
localtime = asctime(localtime(time()))
#print (localtime)
localtime = str(localtime)
localtime = localtime.replace(" ", "_")
print (localtime)

camera = PiCamera()

ledState = GPIO.output (led, GPIO.HIGH)
sleep (0.5)

camera.start_preview()
sleep (1)
scie = '/home/pi/Desktop/Nagrania/'
h = '.h264'
scie = scie+localtime+h
camera.start_recording(scie)

sleep(2)
stop=0
while stop==0:
                e =GPIO.input(buton)
                #print(e)
                
                
                if e==1:
                    
                    bus = smbus.SMBus(1)

# MMA7455L address, 0x1D
                    bus.write_byte_data(0x1D, 0x16, 0x01)
 
    


# Read 6 bytes data back from 0x00
                    data=bus.read_i2c_block_data(0x1D, 0x00, 6)
 
# Convert the data to 10-bits
                    xAcc = (data[1] & 0x03) * 256 + data [0]
                    if xAcc > 511 :
                        xAcc -= 1024
                    yAcc = (data[3] & 0x03) * 256 + data [2]
                    if yAcc > 511 :
                        yAcc -= 1024
                    zAcc = (data[5] & 0x03) * 256 + data [4]
                    if zAcc > 511 :
                        zAcc -= 1024
              
              
              #kalibracja 
                    xAcc = xAcc/67*90+78
                    yAcc = yAcc/67*90+32
                    zAcc = zAcc/67*90-4
    

                    print ("Acceleration in X-Axis : %d" %xAcc)
                    print ("Acceleration in Y-Axis : %d" %yAcc)
                    print ("Acceleration in Z-Axis : %d" %zAcc)
                    clkState = GPIO.input(clk)
                    dtState = GPIO.input(dt)
                    if clkState != clkLastState:
                            if dtState != clkState:
                                    counter += 1
                                
                            else:
                                    counter -= 1
                                    
                    
                    camera.annotate_text=("BY KN SKALNIK  Kliki: %i cm X: %i Y: %i Z: %i" %(counter*3, xAcc, yAcc, zAcc))
                   
                    
                   # print (counter)    
                    clkLastState = clkState
                    sleep(0.1)
                    stop=0
                else:
                    stop=1
                    camera.stop_preview()
                    camera.stop_recording()
sleep(2)    




swState = GPIO.output (led, GPIO.LOW)

