#!/usr/bin/env bash

# ================================================================
# Sentinel Signage Display Unit Install Script
# ================================================================

SENTINEL_USER=sentinel
HOSTNAME_PREFIX="STL-SD"
MAC_ADDRESS=$(cat /sys/class/net/$(ip route show default | awk '/default/ {print $5}')/address | sed 's/://g' | tr [:lower:] [:upper:])
NEW_HOSTNAME="${HOSTNAME_PREFIX}-${MAC_ADDRESS}"

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Signage Display Unit Install Script"
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
info "Starting Signage Display Unit Install Script"
warn "Standard output is sent to /dev/null"


# TODO - install sentinel signage display unit application



info "Setting new hostname ..."
echo ${NEW_HOSTNAME} > /etc/hostname
sed -i "s/127.0.1.1.*${CURRENT_HOSTNAME}/127.0.1.1\t${NEW_HOSTNAME}/g" /etc/hosts
hostname ${NEW_HOSTNAME}
sleep 5

echo -e ""
info "------------------------------------------------------------"
info " Signage Display Unit Install Script Finished"
info " Reboot is required!"
info "------------------------------------------------------------"
echo -e ""
