#!/usr/bin/env bash

# ================================================================
# Sentinel Gateway Install Script
# ================================================================

SENTINEL_USER=sentinel
NEW_HOSTNAME="STL-GW"
IP_ADDRESS=$(hostname -I | awk '{ print $1 }') # IP_ADDRESS=$(hostname -I | cut -d " " -f1)

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Gateway Install Script"
info "------------------------------------------------------------"
if [ $(logname) != ${SENTINEL_USER} ]; then 
  error "Please login as user sentinel to run this script"
fi
if [ $(id -u) -ne 0 ]; then
  error "Please run this script with sudo"
fi
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

echo -e ""
info "Starting Gateway Install Script"
warn "Standard output is sent to /dev/null"


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
sed -i "s/127.0.1.1.*${CURRENT_HOSTNAME}/127.0.1.1\t${NEW_HOSTNAME}/g" /etc/hosts
hostname ${NEW_HOSTNAME}
sleep 5

echo -e ""
info "------------------------------------------------------------"
info " Gateway Install Script Finished"
info " Reboot is required!"
info "------------------------------------------------------------"
echo -e ""
