#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# @description      Display an image
# @author           ben
# @date             2022/12/30
#
# @see              Adafruit Blinka with support for CircuitPython libraries
#                   CircuitPython does not support PIL/pillow (python imaging library)!
#
# pyright: reportMissingImports=false
# pyright: reportMissingModuleSource=false
import os
import sys
import time
import socket
import optparse
import threading
import subprocess
try:
    import busio
    import adafruit_ssd1306
    from   PIL import Image, ImageDraw, ImageFont
    from   board import SCL, SDA
except Exception as E:
    print("\nERROR: python environment not properly set")
    print(str(E)+"\n")
    sys.exit(1)


# Program functions
class namedpipe():
    # Named pipe channel init
    def __init__(self):
        self.__name = os.path.dirname(__file__) + os.path.sep + 'messages.pipe'
        try:
            os.remove(self.__name)
        except FileNotFoundError:
            pass
        try:
            os.mkfifo(self.__name)
            os.chmod(self.__name, 0o666)
        except Exception as E:
            print("ERROR: "+str(E))
            sys.exit(1)
    # Named pipe read data
    # continuously opens the file to catch client programs closing it on their side
    def read(self):
        with open(self.__name) as Pipe:
            data = Pipe.read().strip()
            if len(data) == 0:
                return None
            return data

class display():
    # Display initialization
    def __init__(self, width, height):
        self.__width  = width
        self.__height = height
        i2c = busio.I2C(SCL, SDA)                                                       # Create the I2C interface
        self.__display = adafruit_ssd1306.SSD1306_I2C(self.__width, self.__height, i2c) # Create the SSD1306 OLED class (width, height, I2C bus)
        self.__font = ImageFont.load_default()
        self.__path = os.path.dirname(__file__)
        self.__threadID   = None
        self.__threadStop = False
    def threadStop(self):
        self.__threadStop = True
        if self.__threadID:
            self.__threadID.join()
    # Clear display
    def clear(self):
        self.__display.fill(0)
        self.__display.show()
    # Display image
    def image(self, name):
        try:
            self.__image = Image.open(self.__path+os.path.sep+name).convert("1")
            self.__display.image(self.__image)
            self.__display.show()
        except Exception as E:
            print('ERROR: '+str(E))
    # Power OFF state
    def poweroff(self):
        screen.image('poweroff.ppm')
    # Ready state
    def ready(self):
        screen.image('ready.ppm')
    # Status Page
    def status(self):
        screen.image('status.ppm')
        self.__font = ImageFont.truetype('DejaVuSansMono.ttf', size=14)
        self.clear()
        self.__threadStop = False
        self.__threadID = threading.Thread(target=self.statusThread)
        self.__threadID.start()
    def statusThread(self):
        draw = ImageDraw.Draw(self.__image)
        while not self.__threadStop:
            # Stats
            IP = socket.gethostbyname(socket.gethostname())
            cmd = '/usr/bin/cut -f 1 -d " " /proc/loadavg'
            CPU = subprocess.check_output(cmd, shell=True).decode("utf-8").strip()
            cmd = "/usr/bin/free -m | /usr/bin/awk 'NR==2{printf \"%s\", $3 }'"
            MemUsage = subprocess.check_output(cmd, shell=True).decode("utf-8").strip()
            # Display draw
            draw.rectangle((0, 0,  self.__width, 15),  outline=0, fill=0)
            draw.rectangle((0, 50, self.__width, 64),  outline=0, fill=0)
            draw.text(( 0,  0), IP,         font=self.__font, fill=255)
            draw.text((10, 50), CPU,        font=self.__font, fill=255)
            draw.text((80, 50), MemUsage,   font=self.__font, fill=255, align="left")
            self.__display.image(self.__image)
            self.__display.show()
            time.sleep(1)

    # Loading gauge
    def loading(self):
        self.image("loading.ppm")
        self.__threadStop = False
        self.__threadID = threading.Thread(target=self.loadingThread)
        self.__threadID.start()
    def loadingThread(self):
        draw = ImageDraw.Draw(self.__image)
        i = 0
        while not self.__threadStop:
            if i==0:
                draw.rectangle((16, 34, 110, 47), outline=0, fill=0)
            else:
                X1 = 10+(10 * i)
                draw.rectangle((X1, 34, X1+5, 47),  outline=1, fill=1)
            self.__display.image(self.__image)
            self.__display.show()
            time.sleep(1)
            if i<9:
                i += 1
            else:
                i = 0


# MAIN - Command endless loop
try:
    # Command line parameter parsing
    parser = optparse.OptionParser(usage=f"%prog [options]", epilog='Service display manager utility')
    parser.add_option('-P', '--poweroff', dest='poweroff', default=False, action='store_true', help='Display poweroff message before shutdown')
    (options, _) = parser.parse_args()
    # init pipe and display
    screen = display(128, 64)
    if options.poweroff:
        screen.poweroff()
        time.sleep(1)
        sys.exit(0)
    pipe = namedpipe()
    screen.loading()
    while True:
        command = pipe.read()
        screen.threadStop()
        if command == 'exit':
            screen.clear()
            sys.exit(0)
        elif command == 'ready':
            screen.ready()
        elif command == 'clear':
            screen.clear()
        elif command == 'poweroff':
            screen.poweroff()
        elif command == 'loading':
            screen.loading()
        elif command == 'status':
            screen.status()
        elif command.startswith('image '):
            imageName = command[6:]
            screen.image(imageName)
        else:
            print(f"Unknown command: {command}")
except KeyboardInterrupt:
    print("\nProgram aborted by user")
    sys.exit(1)
except Exception as E:
    print(f"\nERROR: {str(E)}\n")
    sys.exit(1)
