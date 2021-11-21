#!/usr/bin/env bash



# 



# ================================================================
# Sentinel Access Unit Install Script
# ================================================================

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Access Unit Install Script"
info "------------------------------------------------------------"
if [ $(id -u) -ne 0 ]; then
    error "Please run this script with root permissions"
fi
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

info "Starting Access Unit Install Script"



# install sentinel access unit application



info "------------------------------------------------------------"
info "Access Unit Install Script Finished"
info "XXX"
info "Reboot is required!"
info "Then continue with Post Install Script"
info "------------------------------------------------------------"
