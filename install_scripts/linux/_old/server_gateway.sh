#!/usr/bin/env bash

# ================================================================
# Sentinel Gateway Install Script
# ================================================================

DISTRO=$(lsb_release -si)
SENTINEL_USER=sentinel
NEW_HOSTNAME="STL-GW"
# IP_ADDRESS=$(hostname -I | awk '{ print $1 }') # IP_ADDRESS=$(hostname -I | cut -d " " -f1)

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Gateway Install Script"
info "------------------------------------------------------------"
if [ $(uname -m) != "aarch64" ]; then
  error "Please use 64bit architecture"
fi
# if [ $(logname) != ${SENTINEL_USER} ]; then 
#   error "Please login as user sentinel to run this script"
# fi
if [ $(id -u) -ne 0 ]; then
  error "Please run this script with sudo"
fi
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

echo -e ""
info "Starting Gateway Install Script"
warn "Standard output is sent to /dev/null"

case $DISTRO in # ? silent output
  "Debian")
    info "Installing mongodb ..."
    wget -qO - https://www.mongodb.org/static/pgp/server-5.0.asc | apt-key add - #1>/dev/null
    sleep 5
    echo "deb http://repo.mongodb.org/apt/debian buster/mongodb-org/5.0 main" | tee /etc/apt/sources.list.d/mongodb-org-5.0.list
    sleep 5
    apt-get update -y 1>/dev/null
    sleep 5
    apt-get install -y mongodb-org 1>/dev/null
    sleep 5

    info "Enabling mongodb service ..."
    systemctl -q daemon-reload
    sleep 5
    systemctl -q enable mongod
    sleep 5
    systemctl -q start mongod
    sleep 5
  ;;
  "Ubuntu")
    info "Installing mongodb ..."
    wget -qO - https://www.mongodb.org/static/pgp/server-5.0.asc | apt-key add - #1>/dev/null
    sleep 5
    echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu focal/mongodb-org/5.0 multiverse" | tee /etc/apt/sources.list.d/mongodb-org-5.0.list
    sleep 5
    apt-get update -y 1>/dev/null
    sleep 5
    apt-get install -y mongodb-org 1>/dev/null
    sleep 5

    info "Enabling mongodb service ..."
    systemctl -q daemon-reload
    sleep 5
    systemctl -q enable mongod
    sleep 5
    systemctl -q start mongod
    sleep 5
  ;;
  *)
    error "$DISTRO is not supported."
  ;;
esac


# TODO - install sentinel Gateway application

# TODO - create network share -> sentinel_gateway/files/network_share



# change DNS Servers # TODO - if needed
# sudo nano /etc/resolv.conf

# change IP settings # TODO - if needed
# sudo nano /etc/dhcpcd.conf
# https://pimylifeup.com/raspberry-pi-static-ip-address/
# https://thepihut.com/blogs/raspberry-pi-tutorials/how-to-give-your-raspberry-pi-a-static-ip-address-update



info "Setting new hostname ..."
echo ${NEW_HOSTNAME} > /etc/hostname
sed -i "s/${CURRENT_HOSTNAME}/${NEW_HOSTNAME}/g" /etc/hosts
hostname ${NEW_HOSTNAME}
sleep 5

echo -e ""
info "------------------------------------------------------------"
info " Gateway Install Script Finished"
info " Reboot is required!"
info "------------------------------------------------------------"
echo -e ""
