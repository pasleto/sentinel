#!/usr/bin/env bash



HOSTNAME_PREFIX="SENTINEL-SIGNAGE-DISPLAY"
CURRENT_USER=sentinel



# ================================================================
# Sentinel Signage Display Unit Install Script
# ================================================================

MAC_ADDRESS=$(cat /sys/class/net/eth0/address | sed 's/://g' | tr [:lower:] [:upper:])
NEW_HOSTNAME="${HOSTNAME_PREFIX}-${MAC_ADDRESS}"

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Signage Display Unit Install Script"
info "------------------------------------------------------------"
if [ $(id -u) -ne 0 ]; then # TODO - check if user has id 1111 (sentinel)
    error "Please run this script with root permissions"
fi
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

info "Starting Signage Display Unit Install Script"



# TODO - install sentinel signage display unit application



# set new hostname
info "Setting new hostname ..."
rm /etc/hostname
touch /etc/hostname
chown ${CURRENT_USER}:${CURRENT_USER} /etc/hostname
{
echo ${NEW_HOSTNAME}
} >> /etc/hostname
chown root:root /etc/hostname

rm /etc/hosts
touch /etc/hosts
chown ${CURRENT_USER}:${CURRENT_USER} /etc/hosts
{
echo '127.0.0.1       localhost'
echo '::1             localhost ip6-localhost ip6-loopback'
echo 'ff02::1         ip6-allnodes'
echo 'ff02::2         ip6-allrouters'
echo ''
echo '127.0.1.1       '${NEW_HOSTNAME}
} >> /etc/hosts
chown root:root /etc/hosts

hostname ${NEW_HOSTNAME}

info "------------------------------------------------------------"
info "Signage Display Unit Install Script Finished"
info "XXX"
info "Reboot is required!"
info "Then continue with Post Install Script"
info "------------------------------------------------------------"
