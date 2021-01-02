#!/bin/bash

set -e

# Add the user to the GPIO group.
sudo groupadd -f -r gpio
sudo usermod -a -G gpio $USER

# Load the rules. 
sudo cp /opt/nvidia/jetson-gpio/etc/99-gpio.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
