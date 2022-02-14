#!/usr/bin/env bash



DEFAULT_USER=pi
CURRENT_USER=sentinel



# ================================================================
# Sentinel Post Install Script
# ================================================================

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Post Install Script"
info "------------------------------------------------------------"
if [ $(id -u) -ne 0 ]; then # TODO - check if user has id 1111 (sentinel)
    error "Please run this script with root permissions"
fi
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

info "Starting Post Install Script"

# delete default user
info "Deleting default user ..."
egrep "^${DEFAULT_USER}" /etc/passwd >/dev/null
if [ $? -eq 0 ]; then
  userdel -r ${DEFAULT_USER}
else
  warn "User ${DEFAULT_USER} does not exist"
fi



info "------------------------------------------------------------"
info "Post Install Script Finished"
info "Reboot is required!"
info "------------------------------------------------------------"
