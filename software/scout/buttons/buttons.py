#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# @description      Manage handmade keyboard and deals with infoDisplay
# @author           ben
# @date             2023/01/03
#
# @see              (pip) Rpi.GPIO    Install it as Adafruit-PureIO dependency
#
# pyright: reportMissingImports=false
# pyright: reportMissingModuleSource=false
import os
import sys
import stat
import time
import signal
import optparse
import subprocess
try:
    import RPi.GPIO as GPIO
except Exception as E:
    print("\nERROR: python environment not properly set")
    print(str(E)+"\n")
    sys.exit(1)


class stateMachine():
    def __init__(self):
        self.__status = [{
            'name' : 'status',              # Menu Name
            'item' : 'status',              # Action: on display/select the item
            'prev' : ['menu','reboot'],     # Move to previous action
            'next' : ['menu','shutdown'],   # Move to next action
            'enter': None                   # Action: press Enter on menu
        }, {
            'name' : 'shutdown',
            'item' : 'image menushutdown.ppm',
            'prev' : ['menu','status'],
            'next' : ['menu','reboot'],
            'enter': ['menu','menushutdown']
        }, {
            'name' : 'reboot',
            'item' : 'image menureboot.ppm',
            'prev' : ['menu','shutdown'],
            'next' : ['menu','status'],
            'enter': ['menu','menureboot']

        }, {
            'name' : 'menushutdown',
            'item' : 'image confirm.ppm',
            'prev' : ['menu','shutdown'],
            'next' : ['menu','shutdown'],
            'enter': ['function','/usr/bin/poweroff']
        }, {
            'name' : 'menureboot',
            'item' : 'image confirm.ppm',
            'prev' : ['menu','reboot'],
            'next' : ['menu','reboot'],
            'enter': ['function','/usr/bin/reboot']
        }]
        self.__pos = 0
    # get information from current item
    def __action(self, actionName, actionReturn=None):
        item = self.__status[self.__pos][actionName]
        if isinstance(item, list) and item[0] == 'menu':
            newPos = 0
            for element in self.__status:
                if element['name'] == item[1]:
                    self.__pos = newPos
                    if actionReturn:
                        return self.__status[self.__pos][actionReturn]
                    return self.__status[self.__pos][actionName]
                newPos += 1
            return None
        return item
    # Action: Current item load()
    def item(self):
        return self.__action('item')
    # Action: Enter
    def enter(self):
        return self.__action('enter', actionReturn='item')
    # Action: Next
    def next(self):
        return self.__action('next')
    # Action: Previous
    def prev(self):
        return self.__action('prev')

class keyboard():
    # Display initialization
    def __init__(self, namedPipe=None, pinEnter=0, pinUp=0, pinDown=0):
        # Named pipe detection
        if not stat.S_ISFIFO(os.stat(namedPipe).st_mode):
            raise Exception(f"'{namedPipe}' is not a pipe")
        self.__pipeName = namedPipe
        self.__status = stateMachine()
        self.menuEvaluate(self.__status.item())
        # GPIO
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(pinUp,    GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.setup(pinDown,  GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.setup(pinEnter, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.add_event_detect(pinUp,    GPIO.BOTH, callback=self.actionUp,    bouncetime=800)
        GPIO.add_event_detect(pinDown,  GPIO.BOTH, callback=self.actionDown,  bouncetime=800)
        GPIO.add_event_detect(pinEnter, GPIO.BOTH, callback=self.actionEnter, bouncetime=800)

    def menuEvaluate(self, action):
        if isinstance(action, str):
            self.menuDisplay(action)
        elif isinstance(action, list) and action[0]=='function' and isinstance(action[1], str):
            subprocess.check_output(action[1], shell=True).decode("utf-8").strip()

    def menuDisplay(self, command):
        with open(self.__pipeName, 'w') as f:
            f.write(command+'\n')
            f.close()

    def actionUp(self, _channel):
        self.__status.prev()
        self.menuEvaluate(self.__status.item())
    def actionDown(self, _channel):
        self.__status.next()
        self.menuEvaluate(self.__status.item())
    def actionEnter(self, _channel):
        self.menuEvaluate(self.__status.enter())

    def mainLoop(self):
        signal.signal(signal.SIGINT, self.signal_handler)
        signal.pause()

    # might be used as class destructor too because it's the only way of closing this program        
    def signal_handler(self, sig, frame):
        print("\n\nUser input detected, closing program\n")
        GPIO.cleanup()
        sys.exit(0)


# MAIN - Command endless loop
try:
    # Command line parameter parsing
    parser = optparse.OptionParser(usage=f"%prog [options]", epilog='Service display manager utility')
    parser.add_option('-D', '--display', dest='displayPipe', default=None, help='Service display named pipe filename')
    parser.add_option('-S', '--sleep',   dest='sleep',       default=0,    help='Sleep secs before startup (Default:0)')
    (options, _) = parser.parse_args()
    if not options.displayPipe:
        print("Missing option, see --help for usage")
        sys.exit(1)
    if os.getuid() != 0:
        print("This script must be run as 'root' user")
        sys.exit(1)
    # init pipe and display
    time.sleep(int(options.sleep))
    myKeyboard = keyboard(namedPipe=options.displayPipe, pinUp=10, pinDown=12, pinEnter=8)
    myKeyboard.mainLoop()
except Exception as E:
    print(f"\nERROR: {str(E)}\n")
    sys.exit(1)
