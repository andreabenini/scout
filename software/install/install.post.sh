#!/usr/bin/env bash
#
# Post installation script
#

# Default settings
RPIDEV=/dev/sdb


# Business logic
#----------------------------------------------------------------------------------------
if [[ "`which tar`" == "" ]]; then
    echo "'tar' command line utility not found"
    echo "    Install it or use 'tar' instead (and ignore warnings)"
    exit 1
fi

# Installation device selection ($RPIDEV)
if [[ "$1" != "" ]]; then
    RPIDEV=$1
else
    read -p "Raspberry Device (ex: $RPIDEV): " CHOICE
    if [[ "$CHOICE" != "" ]]; then
        RPIDEV=$CHOICE
    fi
fi
echo -en "Do you want to process post install operations on '$RPIDEV' [y|N] ? "
read CHOICE
if [[ "$CHOICE" != "y" ]]; then
    echo "Installation aborted"
    exit 1
fi
read -p "Continue [y|N] ? " CHOICE
if [[ "$CHOICE" != "y" ]]; then
    echo "Installation aborted"
    exit 1
fi
echo ""

# Mount and copy filesystem in the newly created disk
echo "- Mounting root volume"
MNT=./root
mkdir -p $MNT
sudo umount ${RPIDEV}1 ${RPIDEV}2 2>/dev/null
sudo mount ${RPIDEV}2 $MNT
sudo mount ${RPIDEV}1 $MNT/boot

# ./etc/ post install operations
echo "- Post install operations on /etc/"
sudo sh -c "echo 'scout' > $MNT/etc/hostname"
# sshd root access
# sudo sed -i 's/#*\(PermitRootLogin\).*/\1 yes/' $MNT/etc/ssh/sshd_config
echo "      . WiFi setup"
read -p "            Network name: " WIFI_NETWORK
read -p "            WPA Password: " WIFI_PASSWORD
FILENAME=$MNT/etc/netctl/wlan0
sudo sh -c "cat <<EOT > $FILENAME
Description='Automatically generated from the installation process'
Interface=wlan0
Connection=wireless
Security=wpa
ESSID=$WIFI_NETWORK
IP=dhcp
Key=$WIFI_PASSWORD
EOT"
sudo chmod 600 $FILENAME
# Disabling annoying audit messages
sudo sed -i 's/rootwait/rootwait audit=0/' $MNT/boot/cmdline.txt
# Enable wireless profile
sudo rm -f "$MNT/etc/systemd/system/multi-user.target.wants/netctl@wlan0.service"
sudo ln -s '/usr/lib/systemd/system/netctl@.service' "$MNT/etc/systemd/system/multi-user.target.wants/netctl@wlan0.service"
# Renaming user 'alarm' to 'scout'
if [[ -d $MNT/home/alarm ]]; then
    sudo mv "$MNT/home/alarm" "$MNT/home/scout"
fi
sudo sed -i 's/alarm/scout/g' $MNT/etc/passwd
sudo sed -i 's/alarm/scout/g' $MNT/etc/gshadow
sudo sed -i 's/alarm/scout/g' $MNT/etc/group
# ...and changing default password to 'scout'
sudo sed -i 's/^alarm.*/scout:$6$pmp7s.C6a\/P8bU0A$AHN6rsLjNVsqhUdxF2x0JCmcHGg2NfLgZTMy2SY3qcN8lpscNQlGu4q.7V7rT3JIMEXPnFpE5gr\/fE4D9dnFw.:19214:0:99999:7:::/' $MNT/etc/shadow


# Umounting
echo "- Umounting $RPIDEV devices"
sudo sync
sleep 1
sudo umount $MNT/boot
sudo sync
sleep 1
sudo umount $MNT
rmdir -p $MNT 2>/dev/null
echo -e "\nInstallation Completed\n"
