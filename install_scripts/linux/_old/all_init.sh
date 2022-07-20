#!/usr/bin/env bash

# ================================================================
# Sentinel Init Install Script
# ================================================================

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Init Install Script"
info "------------------------------------------------------------"
# if [ $(uname -m) != "aarch64" ]; then
#   error "Please use 64bit architecture"
# fi
if [ $(id -u) -ne 0 ]; then
  error "Please run this script with root permissions"
fi
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

echo -e ""
info "Starting Init Install Script"
warn "Standard output is sent to /dev/null"

info "Running apt-get update ..."
apt-get update -y 1>/dev/null
sleep 5

info "Running apt-get upgrade ..."
apt-get upgrade -y 1>/dev/null
sleep 5

info "Running apt-get autoremove ..."
apt-get autoremove -y 1>/dev/null
sleep 5

info "Downloading sentinel install scripts ..."

# TODO - production
# mkdir -p /opt/sentinel/install
# curl -sLO 'https://files.pasleto.eu/sentinel/deploy/sentinel-units/linux/sentinel-install-scripts/latest.zip' -o /opt/sentinel/install/sentinel-install-scripts.zip
# unzip /opt/sentinel/install/sentinel-install-scripts.zip
# sleep 5

# TODO - test only
mkdir -p /opt/sentinel/install/sentinel-install-scripts
curl -sL 'https://files.pasleto.eu/sentinel/deploy/install-scripts/all_pre_install.txt' -o /opt/sentinel/install/sentinel-install-scripts/all_pre_install.sh
chmod +x /opt/sentinel/install/sentinel-install-scripts/all_pre_install.sh
curl -sL 'https://files.pasleto.eu/sentinel/deploy/install-scripts/unit_storage_terminal.txt' -o /opt/sentinel/install/sentinel-install-scripts/unit_storage_terminal.sh
chmod +x /opt/sentinel/install/sentinel-install-scripts/unit_storage_terminal.sh
sleep 5

echo -e ""
info "------------------------------------------------------------"
info "Init Install Script Finished"
info "Reboot is required! -> Automatic reboot after 10 seconds!"
info "------------------------------------------------------------"
echo -e ""

sleep 10

reboot
