import RPi.GPIO as gp
import os
import time
from time import sleep
from picamera import *


gp.setwarnings(False)
gp.setmode(gp.BOARD)

gp.setup(7, gp.OUT)
gp.setup(11, gp.OUT)
gp.setup(12, gp.OUT)

#gp.setup(15, gp.OUT)
#gp.setup(16, gp.OUT)
#gp.setup(21, gp.OUT)
#gp.setup(22, gp.OUT)

gp.output(7, True)
gp.output(11, True)
gp.output(12, True)
#gp.output(15, True)
#gp.output(16, True)
#gp.output(21, True)
#gp.output(22, True)



def main():
    gp.output(7, False)
    gp.output(11, False)
    gp.output(12, True)
    camera1=PiCamera(camera_num=0)
    camera1.start_preview()
    sleep(1)
    camera1.stop_preview()
    #capture("1")

    gp.output(7, True)
    gp.output(11, False)
    gp.output(12, True)
    camera2=PiCamera(camera_num=1)
    camera2.start_preview()
    sleep(1)
    camera2.stop_preview()
    #capture("2")
    
    gp.output(7, True)
    gp.output(11, True)
    gp.output(12, False)
    #camera4=PiCamera()
    #capture("3")

    gp.output(7, False)
    gp.output(11, True)
    gp.output(12, False)
    #camera3=PiCamera()
    #capture("FishEye")

   
def capture(cam):
    
    date = str(time.strftime("%Y,%m,%d,%H,%M,%S"))
    milisec = str(round(time.time() * 1000))
    camera=PiCamera()
    camera.capture('/home/pi/Desktop/Cam:_%s-D:_%s-Ms:_%s.jpg' %(cam,date,milisec))
    camera.close()    
    #camera.capture('/home/pi/Desktop/Nagrania/%s%s.jpg' %(strings,milisec))
    #t = strings.split(',')
    #numbers = [ int(x) for x in t ]
    

    
    #cmd = "raspistill -o capture_%d%smilisec:%s.jpg -t 250 -r -n -vs" %(cam,strings,milisec)
    #os.system(cmd)
    print(milisec)

if __name__ == "__main__":
    main()

    gp.output(7, False)
    gp.output(11, False)
    gp.output(12, True)