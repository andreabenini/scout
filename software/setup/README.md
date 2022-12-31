# System Setup


## Required Packages
These packages are needed for barebone software setup, everything else will
be installed as a dependency
- git
- gcc
- rsync
- i2c-tools  
    I2C Devices
- python
- python-pillow  
    I2C oled display


## I2C Setup
**/boot/config.txt**
```
dtparam=i2c_arm=on
```
**/etc/modules-load.d/raspberrypi.conf**, add i2c kernel module to this file
```sh
i2c-dev
```
### System check
- `lsmod |grep i2c_dev`
- `ls -la /dev/i2c-1`, might be _i2c-0_ if you've an older machine. 
    **i2c-20**, **i2c-21** are the HDMI output ports
- `i2cdetect -y 1`, where **1** is related to `i2c-1`, change it accordingly.
    This should print the I2C setup on GPIO port 1
- `i2cdetect -l`, displays the list of I2C Buses available in your system. Reference
    to i2c-20 and i2c-21 to **Broadcom STB** are related to HDMI ports


## Python Virtual Enviornment
These packages are needed, create a virtual environment and install them all in this order.
There's no need to execute everything in a privileged environment, normal user is preferred.
- wheel
- adafruit-circuitpython-ssd1306  
    I2C oled display
- RPi-GPIO
- Pillow  
    I2C oled graphical capabilities and animations

### I2C Display Samples
- **PREFERRED**, use this library for some OLED display samples:  
    https://github.com/adafruit/Adafruit_CircuitPython_SSD1306  
    This one is using CircuitPython libs
- **DEPRECATED**, do _NOT_ use this one:  
    https://github.com/adafruit/Adafruit_Python_SSD1306  
    this lib is still using the old _python-wiring_ as backend, it has
    been discontinued from RPi Foundation, use circuit python based one


