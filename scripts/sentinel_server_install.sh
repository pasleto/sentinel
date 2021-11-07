#!/usr/bin/env bash

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel Server - Install Script"
info "------------------------------------------------------------"
if [ $(id -u) -ne 0 ]; then
    error "Please run this script with root permissions"
fi
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10
info "Starting Prerequisites Script"

DEFAULT_USER=pi
CURRENT_USER=sentinel

IP_ADDRESS=$(hostname -I | awk '{ print $1 }')

NEW_HOSTNAME="SENTINEL-SERVER"



# delete default user
info "Deleting default user ..."
egrep "^${DEFAULT_USER}" /etc/passwd >/dev/null
if [ $? -eq 0 ]; then
  userdel -r ${DEFAULT_USER}
else
  warn "User ${DEFAULT_USER} does not exist"
fi






# install CMS Server app

# create network share -> cms_server/files/network_share







# change DNS Servers #TODO
# sudo nano /etc/resolv.conf

# change IP settings #TODO
# sudo nano /etc/dhcpcd.conf
# https://pimylifeup.com/raspberry-pi-static-ip-address/
# https://thepihut.com/blogs/raspberry-pi-tutorials/how-to-give-your-raspberry-pi-a-static-ip-address-update



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
info "Sentinel Server is now installed"
info "You can access web interface here:"
info "http://${IP_ADDRESS}:80/"
info "------------------------------------------------------------"