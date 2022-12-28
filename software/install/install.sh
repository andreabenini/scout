#!/usr/bin/env bash
#
# Arch linux installation script for RPi4
#

# Default settings
RPIDEV=/dev/sdb
BOOT_SIZE=200       # Mb
PARTED=`which parted 2>/dev/null`
MKFS=`which mkfs     2>/dev/null`
TAR=`which bsdtar    2>/dev/null`


# Business logic
#----------------------------------------------------------------------------------------
if [[ "$PARTED" == "" ]]; then
    echo "'parted' command line utility not found"
    exit 1
fi
if [[ "$MKFS" == "" ]]; then
    echo "'mkfs' command line utility not found"
    exit 1
fi
if [[ "$TAR" == "" ]]; then
    echo "'bsdtar' command line utility not found"
    echo "    Install it or use 'tar' instead (and ignore warnings)"
    exit 1
fi

# Installation file detection
if [[ ! -f "$1" ]] || [[ $1 != *.tar.gz ]]; then
    echo -e "\nOS installation file not found\nUsage: $0 <installFile> [device]\n"
    exit 1
fi
OS_INSTALL=$1
echo -e "\nInstallation file: $OS_INSTALL\n"
# Installation device selection ($RPIDEV)
if [[ "$2" != "" ]]; then
    RPIDEV=$2
else
    read -p "Raspberry Device (ex: $RPIDEV): " CHOICE
    if [[ "$CHOICE" != "" ]]; then
        RPIDEV=$CHOICE
    fi
fi
echo -en "Do you want to want to install the OS on '$RPIDEV' [y|N] ? "
read CHOICE
if [[ "$CHOICE" != "y" ]]; then
    echo "Installation aborted"
    exit 1
fi
sudo $PARTED -s $RPIDEV print
read -p "Continue [y|N] ? " CHOICE
if [[ "$CHOICE" != "y" ]]; then
    echo "Installation aborted"
    exit 1
fi

# Wiping partitions and initialize device
# Size of the disk:
#       parted -s /dev/sda print|awk '/^Disk/ {print $3}'|sed 's/[Mm][Bb]//'
echo -e "\n- Deleting existing partitions"
sudo $PARTED -s $RPIDEV mklabel msdos
for PARTITION in $(sudo $PARTED -s $RPIDEV print|awk '/^ / {print $1}'); do
    echo "      . Partition $PARTITION"
    sudo $PARTED -s $RPIDEV rm $PARTITION
done
echo "- Creating new partitions"
sudo $PARTED -s $RPIDEV mkpart primary fat32 4  ${BOOT_SIZE}MB
sudo $PARTED -s $RPIDEV mkpart primary ext2  ${BOOT_SIZE}MB  100%
# Format partitions
sudo $MKFS -t vfat -n BOOT ${RPIDEV}1
sudo $MKFS -t ext4 -L root -F ${RPIDEV}2

# Mount and copy filesystem in the newly created disk
echo "- Mounting SD Card"
mkdir -p boot root
sudo mount ${RPIDEV}1 ./boot
sudo mount ${RPIDEV}2 ./root
# OS Installation
echo "- Installing the operating system   (this might take a while...)"
echo "      . $TAR files"
sudo $TAR -xpf $OS_INSTALL -C ./root
echo "      . filesystem sync"
sudo sync
echo "      . moving boot files"
sudo mv root/boot/* boot
# Umounting
echo "- Umounting $RPIDEV devices"
sudo umount ${RPIDEV}1 ${RPIDEV}2
rmdir -p boot root 2>/dev/null
echo -e "\nInstallation Completed\n"
