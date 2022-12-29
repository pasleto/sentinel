#!/usr/bin/env bash

# ================================================================
# Sentinel Gateway Server Install Script
# ================================================================

DISTRO=$(lsb_release -si)
CURRENT_HOSTNAME=$(hostname -s)
CURRENT_USERNAME=$(logname)
SENTINEL_USERNAME=sentinel
# SENTINEL_PASSWORD=password
SENTINEL_PASSWORD=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | head -c 13)
DEFAULT_NEW_HOSTNAME="STL-GW"
NEW_TIMEZONE="UTC"
NTP_SERVERS="time.cloudflare.com time.google.com"
FALLBACK_NTP_SERVERS="time.cloudflare.com time.google.com"
SUPPORTED_ARM_CPUS=("Cortex-A55","Cortex-A75","Cortex-A76","Cortex-A77","Cortex-A78","Cortex-X1","Neoverse-N1","Cortex-A65","Neoverse-E1","Neoverse-V1","Cortex-A510","Cortex-A710","Cortex-X2","Neoverse-N2")

function info { echo -e "\e[32m[i] $*\e[39m"; }
function warn  { echo -e "\e[33m[!] $*\e[39m"; }
function input  { echo -e -n "\e[36m[?] $* \e[39m"; }
function error { echo -e "\e[31m[x] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Gateway Server Install Script"
info "------------------------------------------------------------"
if [ $(id -u) -ne 0 ]; then
  error "Please run this script with sudo"
fi
warn "System should be fully up to date and rebooted before running this script!"
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

echo -e ""
info "Starting Gateway Server Install Script"
warn "Standard output is sent to /dev/null"

info "Checking operating system ..."
case $DISTRO in
  "Debian")
    if [ $(dpkg --print-architecture) != "x86_64" ]; then
      # if [[ ! "${SUPPORTED_ARM_CPUS[*]}" =~ "$(lscpu | grep "Model name" | cut -f 2 -d ":" | awk '{$1=$1}1')" ]]; then
      #   error "$(lscpu | grep "Model name" | cut -f 2 -d ":" | awk '{$1=$1}1') is not supported CPU"
      # fi
      error "Please use desktop architecture (x86_64) of $DISTRO"
    fi
    if [ $(uname -m) != "aarch64" ]; then
      error "Please use 64bit version of $DISTRO"
    fi

    info "Distribution is $DISTRO ..."

    apt-get install -y network-manager 1>/dev/null
  ;;
  "Ubuntu")
    if [ $(dpkg --print-architecture) != "x86_64" ]; then
      if [[ ! "${SUPPORTED_ARM_CPUS[*]}" =~ "$(lscpu | grep "Model name" | cut -f 2 -d ":" | awk '{$1=$1}1')" ]]; then
        error "$(lscpu | grep "Model name" | cut -f 2 -d ":" | awk '{$1=$1}1') is not supported CPU"
      fi
    fi
    if [ $(uname -m) != "aarch64" ]; then
      error "Please use 64bit version of $DISTRO"
    fi

    info "Distribution is $DISTRO ..."
  ;;
  *)
    error "$DISTRO is not supported."
  ;;
esac

input "Hostname [${DEFAULT_NEW_HOSTNAME}]:"
read NEW_HOSTNAME
NEW_HOSTNAME=${NEW_HOSTNAME:-$DEFAULT_NEW_HOSTNAME}

input "Domain name [ ]:"
read NEW_DOMAINNAME_IN
NEW_DOMAINNAME_IN=${NEW_DOMAINNAME_IN:-" "}

case "$NEW_DOMAINNAME_IN" in
  " ")
    NEW_DOMAIN_HOSTNAME=${NEW_HOSTNAME}
    CLOUD_DEPLOY="N"
    AD_DEPLOY="N"
    AD_ADMIN_GROUP="N"
  ;;
  *)
    NEW_DOMAINNAME=$( echo ${NEW_DOMAINNAME_IN} | tr [:upper:] [:lower:] )
    NEW_DOMAIN_HOSTNAME="${NEW_HOSTNAME}.${NEW_DOMAINNAME}"

    input "Join machine to Active Directory domain - Y/N? [N]:"
    read AD_DEPLOY

    case "$AD_DEPLOY" in
      [Yy]*)
        AD_ADMIN_USERNAME_CHECK=true
        while $AD_ADMIN_USERNAME_CHECK; do
          input "Active Directory administrator user principal name [administrator@${NEW_DOMAINNAME}]:"
          read AD_ADMIN_USERNAME
          AD_ADMIN_USERNAME=${AD_ADMIN_USERNAME:-"administrator@${NEW_DOMAINNAME}"}

          upn="^[a-z0-9!#\$%&'*+/=?^_\`{|}~-]+(\.[a-z0-9!#$%&'*+/=?^_\`{|}~-]+)*@([a-z0-9]([a-z0-9-]*[a-z0-9])?\.)+[a-z0-9]([a-z0-9-]*[a-z0-9])?\$"
          if [[ $AD_ADMIN_USERNAME =~ $upn ]] ; then 
            AD_ADMIN_USER=$( echo ${AD_ADMIN_USERNAME} | cut -d "@" -f 1 | tr [:upper:] [:lower:] )
            AD_ADMIN_DOMAIN=$( echo ${AD_ADMIN_USERNAME} | cut -d "@" -f 2 | tr [:lower:] [:upper:] )
            AD_ADMIN_UPN="${AD_ADMIN_USER}@${AD_ADMIN_DOMAIN}"
            AD_ADMIN_USERNAME_CHECK=false
          else
            warn "You need to provide valid user principal name!"
          fi
        done

        AD_ADMIN_PASSWORD_CHECK=true
        while $AD_ADMIN_PASSWORD_CHECK; do
          input "Active Directory administrator password:"
          read AD_ADMIN_PASSWORD
          AD_ADMIN_PASSWORD=${AD_ADMIN_PASSWORD:-" "}

          if [[ $AD_ADMIN_PASSWORD != " " ]] ; then
            AD_ADMIN_PASSWORD_CHECK=false
          else
            warn "You need to provide password!"
          fi
        done

        input "Local admin group name to enable logon alongside Domain Admins and Enterprise Admins [ ]:" # ? check if working
        read AD_ADMIN_GROUP
        AD_ADMIN_GROUP=${AD_ADMIN_GROUP:-" "}
      ;;
      *)
        info "Skipping Active Directory domain join ..."
      ;;
    esac

    input "Setup for cloud deployment - Y/N? [N]:"
    read CLOUD_DEPLOY

    case "$CLOUD_DEPLOY" in
      [Yy]*)
        input "DNS domains to obtain certificate for - comma separated list [${NEW_DOMAIN_HOSTNAME}]:"
        read SSL_DOMAIN
        SSL_DOMAIN=${SSL_DOMAIN:-$NEW_DOMAIN_HOSTNAME}

        warn "You need to make sure that public DNS A record for [${SSL_DOMAIN}] is pointing to this server! Otherwise the certificate request will fail!"

        MAIL_CHECK=true
        while $MAIL_CHECK; do
          input "Email address for Lets Encrypt Certificate notifications:"
          read SSL_TOS_EMAIL
          SSL_TOS_EMAIL=${SSL_TOS_EMAIL:-" "}

          email="^[a-z0-9!#\$%&'*+/=?^_\`{|}~-]+(\.[a-z0-9!#$%&'*+/=?^_\`{|}~-]+)*@([a-z0-9]([a-z0-9-]*[a-z0-9])?\.)+[a-z0-9]([a-z0-9-]*[a-z0-9])?\$"
          if [[ $SSL_TOS_EMAIL =~ $email ]] ; then
            SSL_DOMAIN_DIR=$( echo ${SSL_DOMAIN} | cut -d "," -f 1 | tr [:upper:] [:lower:] )
            MAIL_CHECK=false
          else
            warn "You need to provide valid email address!"
          fi
        done
      ;;
      *)
        info "Skipping cloud deploy setup ..."
      ;;
    esac
  ;;
esac

info "Proceeding with installation ..."

# if [ -f "/etc/motd" ]; then
#   info "Removing MOTD ..."
#   rm /etc/motd
# fi

info "Changing timezone ..."
timedatectl set-timezone ${NEW_TIMEZONE}

info "Changing RTC into UTC zone ..."
timedatectl set-local-rtc 0

info "Setting up new NTP configuration ..."
sed -i "/Fallback/!s/.*NTP=.*/NTP=${NTP_SERVERS}/g" /etc/systemd/timesyncd.conf
sed -i "/Fallback/s/.*FallbackNTP=.*/FallbackNTP=${FALLBACK_NTP_SERVERS}/g" /etc/systemd/timesyncd.conf
sed -i "s/.*RootDistanceMaxSec=.*/RootDistanceMaxSec=5/g" /etc/systemd/timesyncd.conf
sed -i "s/.*PollIntervalMinSec=.*/PollIntervalMinSec=32/g" /etc/systemd/timesyncd.conf
sed -i "s/.*PollIntervalMaxSec=.*/PollIntervalMaxSec=2048/g" /etc/systemd/timesyncd.conf
timedatectl set-ntp true
systemctl -q daemon-reload
systemctl -q restart systemd-timesyncd

info "Creating user sentinel ..."
egrep "^${SENTINEL_USERNAME}" /etc/passwd >/dev/null
if [ $? -eq 0 ]; then
  warn "User ${SENTINEL_USERNAME} already exists"
else
  useradd -u 1111 -m ${SENTINEL_USERNAME}

  info "Changing user sentinel password ..."
  echo ${SENTINEL_USERNAME}:${SENTINEL_PASSWORD} | chpasswd

  info "Changing user sentinel shell to bash ..."
  usermod -s /bin/bash ${SENTINEL_USERNAME} >/dev/null

  info "Setting user sentinel login dashboard ..."
  echo '' >> /home/${SENTINEL_USERNAME}/.profile
  echo 'echo "' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) Date......................:$(tput setaf 1) `date +"%A, %d. %B %Y, %T %Z"`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) System....................:$(tput setaf 1) `uname -srmo`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) Uptime....................:$(tput setaf 1) `printf "%d days, %02dh %02dm %02ds" "$(($(cut -d. -f1 /proc/uptime)/86400))" "$(($(cut -d. -f1 /proc/uptime)/3600%24))" "$(($(cut -d. -f1 /proc/uptime)/60%60))" "$(($(cut -d. -f1 /proc/uptime)%60))"`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) Memory....................:$(tput setaf 1) Total: `free -m | grep Mem | awk '\''{print $2}'\''`MB | Used: `free -m | grep Mem | awk '\''{print $3}'\''`MB | Free: `free -m | grep Mem | awk '\''{print $4}'\''`MB' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) Disk Usage................:$(tput setaf 1) Total: `df -h ~ | grep /dev/^* | awk '\''{print $2}'\''`B | Used: `df -h ~ | grep /dev/^* | awk '\''{print $3}'\''`B | Free: `df -h ~ | grep /dev/^* | awk '\''{print $4}'\''`B | Usage: `df -h ~ | grep /dev/^* | awk '\''{print $5}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) Load Averages.............:$(tput setaf 1) `cut -c "1-4" /proc/loadavg` (1 min) | `cut -c "6-9" /proc/loadavg` (5 min) | `cut -c "11-14" /proc/loadavg` (15 min)' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) Running Processes.........:$(tput setaf 1) `ps ax | wc -l | tr -d " "`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) IP Address................:$(tput setaf 1) `hostname -I | cut -d " " -f1`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) Default Gateway...........:$(tput setaf 1) `/sbin/ip route | awk '\''/default/ {print $3}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
  case $DISTRO in
    "Debian")
      echo '$(tput setaf 2) DNS Servers...............:$(tput setaf 1) `nmcli dev show $(/sbin/ip route | awk '\''/default/ {print $5}'\'') | awk '\''/DNS/ {print $2}'\'' | xargs`' >> /home/${SENTINEL_USERNAME}/.profile    
    ;;
    "Ubuntu")
      echo '$(tput setaf 2) DNS Servers...............:$(tput setaf 1) `resolvectl dns | grep $(/sbin/ip route | awk '\''/default/ {print $5}'\'') | awk '\''{split($0,a,": "); print a[2]}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
    ;;
    *)
      echo '$(tput setaf 2) DNS Servers...............:$(tput setaf 1) `cat /etc/resolv.conf | grep '\''nameserver'\'' | awk '\''{ print $2 }'\'' ORS='\'' '\''`' >> /home/${SENTINEL_USERNAME}/.profile
    ;;
  esac
  case $DISTRO in
    "Debian")
      echo '$(tput setaf 2) DNS Domain...............:$(tput setaf 1) `nmcli dev show $(/sbin/ip route | awk '\''/default/ {print $5}'\'') | awk '\''/DOMAIN/ {print $2}'\'' | xargs`' >> /home/${SENTINEL_USERNAME}/.profile    
    ;;
    "Ubuntu")
      echo '$(tput setaf 2) DNS Domain...............:$(tput setaf 1) `resolvectl domain | grep $(/sbin/ip route | awk '\''/default/ {print $5}'\'') | awk '\''{split($0,a,": "); print a[2]}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
    ;;
    *)
      echo '$(tput setaf 2) DNS Domain...............:$(tput setaf 1) `cat /etc/resolv.conf | grep '\''domain'\'' | awk '\''{ print $2 }'\''`' >> /home/${SENTINEL_USERNAME}/.profile
    ;;
  esac
  echo '$(tput setaf 2) MAC Address...............:$(tput setaf 1) `cat /sys/class/net/$(/sbin/ip route | awk '\''/default/ {print $5}'\'')/address | sed '\''s/:/-/g'\'' | tr [:lower:] [:upper:]`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) Default Ethernet Port.....:$(tput setaf 1) `/sbin/ip route | awk '\''/default/ {print $5}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2) Hostname..................:$(tput setaf 1) `hostname -f`' >> /home/${SENTINEL_USERNAME}/.profile
  if [ -f "/sys/class/thermal/thermal_zone0/temp" ]; then
    echo '$(tput setaf 2) CPU Temperature...........:$(tput setaf 1) `cat /sys/class/thermal/thermal_zone0/temp | awk '\''{print $1/1000}'\''`ºC' >> /home/${SENTINEL_USERNAME}/.profile
  fi
  echo '$(tput setaf 2) Updates available.........:$(tput setaf 1) `apt-get dist-upgrade -s --quiet=2 | grep ^Inst | wc -l`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput sgr0)"' >> /home/${SENTINEL_USERNAME}/.profile
  echo '' >> /home/${SENTINEL_USERNAME}/.profile
fi

if [ ! -f "/home/${SENTINEL_USERNAME}/.hushlogin" ]; then
  info "Disabling user sentinel login message ..."
  touch /home/${SENTINEL_USERNAME}/.hushlogin
fi

case $DISTRO in
  "Debian")
    if [ -f "/etc/rpi-issue" ]; then
      info "Changing /boot/config.txt ..."
      if ! grep -q "dtoverlay=disable-wifi" /boot/config.txt; then
        echo "dtoverlay=disable-wifi" >> /boot/config.txt
      fi
      if ! grep -q "dtoverlay=disable-bt" /boot/config.txt; then
        echo "dtoverlay=disable-bt" >> /boot/config.txt
      fi
      if ! grep -q "disable_splash=1" /boot/config.txt; then
        echo "disable_splash=1" >> /boot/config.txt
      fi
      if ! grep -q "dtparam=spi=on" /boot/config.txt; then
        echo "dtparam=spi=on" >> /boot/config.txt
      fi
      if ! grep -q "dtparam=i2c_arm=on" /boot/config.txt; then
        echo "dtparam=i2c_arm=on" >> /boot/config.txt
      fi

      info "Changing /boot/cmdline.txt ..."
      sed -i "s/tty1/tty3/g" /boot/cmdline.txt
      if ! grep -q " quiet" /boot/cmdline.txt; then
        sed -i "s/$/ quiet/" /boot/cmdline.txt
      fi
      if ! grep -q " logo.nologo" /boot/cmdline.txt; then
        sed -i "s/$/ logo.nologo/" /boot/cmdline.txt
      fi
      if ! grep -q " vt.global_cursor_default=0" /boot/cmdline.txt; then
        sed -i "s/$/ vt.global_cursor_default=0/" /boot/cmdline.txt
      fi
      if ! grep -q " consoleblank=0" /boot/cmdline.txt; then
        sed -i "s/$/ consoleblank=0/" /boot/cmdline.txt
      fi

      info "Setting permissions for user sentinel ..."
      usermod -aG adm,dialout,cdrom,sudo,audio,video,plugdev,users,input,netdev,spi,i2c ${SENTINEL_USERNAME} # gpio
    else
      info "Setting permissions for user sentinel ..."
      usermod -aG adm,dialout,cdrom,sudo,audio,video,plugdev,users,netdev ${SENTINEL_USERNAME}
    fi
  ;;
  "Ubuntu")
    if [[ $(cat /proc/cpuinfo | grep "Model" | awk '{ print $3 }') == "Raspberry" ]]; then
      info "Changing /boot/firmware/config.txt ..."
      echo "" >> /boot/firmware/config.txt
      if ! grep -q "dtoverlay=disable-wifi" /boot/firmware/config.txt; then
        echo "dtoverlay=disable-wifi" >> /boot/firmware/config.txt
      fi
      if ! grep -q "dtoverlay=disable-bt" /boot/firmware/config.txt; then
        echo "dtoverlay=disable-bt" >> /boot/firmware/config.txt
      fi
      if ! grep -q "disable_splash=1" /boot/firmware/config.txt; then
        echo "disable_splash=1" >> /boot/firmware/config.txt
      fi
      if ! grep -q "dtparam=spi=on" /boot/firmware/config.txt; then
        echo "dtparam=spi=on" >> /boot/firmware/config.txt
      fi
      if ! grep -q "dtparam=i2c_arm=on" /boot/firmware/config.txt; then
        echo "dtparam=i2c_arm=on" >> /boot/firmware/config.txt
      fi

      info "Changing /boot/firmware/cmdline.txt ..." # ? still some message shown before kernel
      sed -i "s/tty1/ttyS0/g" /boot/firmware/cmdline.txt
      if ! grep -q " quiet" /boot/firmware/cmdline.txt; then
        sed -i "s/$/ quiet/" /boot/firmware/cmdline.txt
      fi
      if ! grep -q " logo.nologo" /boot/firmware/cmdline.txt; then
        sed -i "s/$/ logo.nologo/" /boot/firmware/cmdline.txt
      fi
      if ! grep -q " vt.global_cursor_default=0" /boot/firmware/cmdline.txt; then
        sed -i "s/$/ vt.global_cursor_default=0/" /boot/firmware/cmdline.txt
      fi
      if ! grep -q " consoleblank=0" /boot/firmware/cmdline.txt; then
        sed -i "s/$/ consoleblank=0/" /boot/firmware/cmdline.txt
      fi

      usermod -aG spi,i2c ${SENTINEL_USERNAME} # gpio
    fi

    if [ -f "/etc/cloud/cloud.cfg" ]; then
      info "Disabling cloud init ..."
      touch /etc/cloud/cloud-init.disabled
    fi

    info "Setting permissions for user sentinel ..."
    usermod -aG adm,dialout,cdrom,sudo,audio,video,plugdev,users,netdev ${SENTINEL_USERNAME}
  ;;
  *)
    error "$DISTRO is not supported."
  ;;
esac

info "Installing needed packages ..."
apt-get install -y wget curl git ufw unzip 1>/dev/null
apt-get install -y ca-certificates software-properties-common apt-transport-https gnupg lsb-release 1>/dev/null

case "$AD_DEPLOY" in
  [Yy]*)
    info "Installing packages needed for Active Directory domain join ..."
    apt-get install -y realmd sssd packagekit 1>/dev/null

    info "Joining machine to Active Directory domain ..." # ? check if already joined -> check if output of realm list is empty or not
    # ? realm list --name-only
    echo ${AD_ADMIN_PASSWORD} | realm join ${NEW_DOMAINNAME} -U "${AD_ADMIN_UPN}" --os-name "$(lsb_release -si)" --os-version "$(lsb_release -sr)" 1>/dev/null
    # TODO - what happen when this host already exist in AD

    info "Allowing Domain Admins and Enterprise Admins to login ..."
    realm deny --all
    realm permit --groups "Domain Admins" "Enterprise Admins"

    case "$AD_ADMIN_GROUP" in # ? validate if working
      " ")
        info "Skipping local admin group in logon permissions ..."
      ;;
      *)
        info "Allowing ${AD_ADMIN_GROUP} to login ..."
        realm permit --groups "${AD_ADMIN_GROUP}"
      ;;
    esac
  ;;
  *)
    info "Skipping Active Directory domain join ..."
  ;;
esac

case "$CLOUD_DEPLOY" in
  [Yy]*)
    info "Installing packages needed for cloud setup ..."
    apt-get install -y snapd 1>/dev/null
    snap install core 1>/dev/null
    snap refresh core &>/dev/null
    snap install --classic certbot 1>/dev/null
    ln -s /snap/bin/certbot /usr/bin/certbot 1>/dev/null

    info "Opening needed ports in firewall ..."
    ufw allow 80/tcp 1>/dev/null
    ufw --force enable 1>/dev/null

    info "Retrieving letsencrypt certificate for ${SSL_DOMAIN} ..."
    if [ ! -d "/etc/letsencrypt/live/${SSL_DOMAIN_DIR}" ]; then # ? - check if /etc/letsencrypt/${SSL_DOMAIN_DIR} exist, if not run certbot, otherwise skip or try renew
      certbot certonly --standalone --non-interactive --domains ${SSL_DOMAIN} --agree-tos --email ${SSL_TOS_EMAIL} --quiet
    fi

    # --cert-path /etc/letsencrypt/sentinel --key-path /etc/letsencrypt/sentinel --fullchain-path /etc/letsencrypt/sentinel --chain-path /etc/letsencrypt/sentinel
    # --work-dir /etc/letsencrypt/sentinel --config-dir /etc/letsencrypt/sentinel --logs-dir /etc/letsencrypt/sentinel 

    sudo certbot renew --dry-run --quiet
  ;;
  *)
    info "Skipping cloud setup ..."
  ;;
esac

info "Installing nodejs ..."
curl -sL https://deb.nodesource.com/setup_lts.x | bash - 1>/dev/null
apt-get install -y nodejs 1>/dev/null

case $DISTRO in
  "Debian")
    info "Installing mongodb ..."
    mkdir -p /etc/apt/keyrings
    curl -fsSL https://www.mongodb.org/static/pgp/server-6.0.asc | tee -a /etc/apt/keyrings/mongodb-org-6_0.asc > /dev/null
    echo "deb [signed-by=/etc/apt/keyrings/mongodb-org-6_0.asc] http://repo.mongodb.org/apt/debian $(lsb_release -sc)/mongodb-org/6.0 main" | tee /etc/apt/sources.list.d/mongodb-org-6.0.list > /dev/null
    apt-get update -y 1>/dev/null
    apt-get install -y mongodb-org 1>/dev/null
    info "Enabling mongodb service ..."
    systemctl -q daemon-reload
    systemctl -q enable mongod
    systemctl -q start mongod
  ;;
  "Ubuntu")
    info "Installing mongodb ..."
    mkdir -p /etc/apt/keyrings
    curl -fsSL https://www.mongodb.org/static/pgp/server-6.0.asc | tee -a /etc/apt/keyrings/mongodb-org-6_0.asc > /dev/null
    echo "deb [arch=amd64,arm64 signed-by=/etc/apt/keyrings/mongodb-org-6_0.asc] https://repo.mongodb.org/apt/ubuntu $(lsb_release -sc)/mongodb-org/6.0 multiverse" | tee /etc/apt/sources.list.d/mongodb-org-6.0.list > /dev/null
    apt-get update -y 1>/dev/null
    apt-get install -y mongodb-org 1>/dev/null
    info "Enabling mongodb service ..."
    systemctl -q daemon-reload
    systemctl -q enable mongod
    systemctl -q start mongod
  ;;
  *)
    error "$DISTRO is not supported."
  ;;
esac

info "Creating application paths ..."
mkdir -p /opt/sentinel/install
mkdir -p /opt/sentinel/application

info "Downloading sentinel gateway application ..."
pushd /opt/sentinel/install/ 1>/dev/null
curl -sLO 'https://github.com/pasleto/sentinel/raw/main/install_scripts/linux/content/sentinel_gateway.zip'
unzip -qu sentinel_gateway.zip -d ./sentinel_gateway
popd 1>/dev/null

info "Installing sentinel gateway application ..."
cp -r /opt/sentinel/install/sentinel_gateway/. /opt/sentinel/application/
pushd /opt/sentinel/application/ 1>/dev/null
/usr/bin/npm install &>/dev/null
popd 1>/dev/null

info "Opening needed ports in firewall ..."
ufw allow 443/tcp 1>/dev/null
ufw --force enable 1>/dev/null

if [ ! -f "/etc/systemd/system/sentinel-gateway.service" ]; then # ? - validate
  info "Creating sentinel gateway service ..."
  echo '' >> /etc/systemd/system/sentinel-gateway.service # touch /etc/systemd/system/sentinel-gateway.service
  echo '[Unit]' >> /etc/systemd/system/sentinel-gateway.service
  echo 'Description=Sentinel Gateway' >> /etc/systemd/system/sentinel-gateway.service
  echo 'After=network.target' >> /etc/systemd/system/sentinel-gateway.service
  echo '' >> /etc/systemd/system/sentinel-gateway.service
  echo '[Service]' >> /etc/systemd/system/sentinel-gateway.service
  echo 'Environment=NODE_ENV=production' >> /etc/systemd/system/sentinel-gateway.service
  echo 'Type=Simple' >> /etc/systemd/system/sentinel-gateway.service
  echo 'User='${SENTINEL_USER} >> /etc/systemd/system/sentinel-gateway.service
  echo 'Restart=on-failure' >> /etc/systemd/system/sentinel-gateway.service
  echo 'WorkingDirectory=/opt/sentinel/application' >> /etc/systemd/system/sentinel-gateway.service
  echo 'ExecStart=/usr/bin/node index.js' >> /etc/systemd/system/sentinel-gateway.service
  echo '' >> /etc/systemd/system/sentinel-gateway.service
  echo '[Install]' >> /etc/systemd/system/sentinel-gateway.service
  echo 'WantedBy=multi-user.target' >> /etc/systemd/system/sentinel-gateway.service
fi
# echo 'ExecStart=/usr/bin/node /opt/sentinel/application/index.js' >> /etc/systemd/system/sentinel-gateway.service
# echo 'ExecStart=npm start' >> /etc/systemd/system/sentinel-gateway.service # /usr/bin/npm
# echo 'ExecStart=cross-env NODE_ENV=production node /opt/sentinel/application/index.js' >> /etc/systemd/system/sentinel-gateway.service # /usr/bin/node

case "$CLOUD_DEPLOY" in
  [Yy]*)
    if [ -d "/etc/letsencrypt/live/${SSL_DOMAIN_DIR}" ]; then
      if [ ! -d "/opt/sentinel/application/certificates-orig" ]; then
        mv /opt/sentinel/application/certificates /opt/sentinel/application/certificates-orig
        info "Creating symlink for letsencrypt certificates due to cloud setup ..."
        ln -s /etc/letsencrypt/live/${SSL_DOMAIN_DIR} /opt/sentinel/application/certificates
      fi
    fi
  ;;
  *)
    info "Skipping cloud certificate setup, using default certificates ..."
  ;;
esac

info "Enabling and starting sentinel gateway service ..."
systemctl -q enable sentinel-gateway.service
systemctl -q start sentinel-gateway.service



case "$NEW_DOMAINNAME_IN" in
  " ")
    SAMBA_DOMAIN="WORKGROUP"
  ;;
  *)
    SAMBA_DOMAIN=$( echo ${NEW_DOMAINNAME} | tr [:lower:] [:upper:] )
  ;;
esac

# ? - create network share -> sentinel_gateway/files/network_share



info "Running apt-get update ..."
apt-get update -y 1>/dev/null

info "Running apt-get upgrade ..."
apt-get upgrade -y 1>/dev/null

info "Running apt-get autoremove ..."
apt-get autoremove -y 1>/dev/null

info "Setting new hostname ..."
echo ${NEW_HOSTNAME} > /etc/hostname
case "$NEW_DOMAINNAME_IN" in
  " ")
    sed -i "s/${CURRENT_HOSTNAME}/${NEW_HOSTNAME}/g" /etc/hosts
  ;;
  *)
    sed -i "s/${CURRENT_HOSTNAME}/${NEW_DOMAIN_HOSTNAME}\t${NEW_HOSTNAME}/g" /etc/hosts
  ;;
esac
hostname ${NEW_HOSTNAME}

if ! grep -q "net.ipv6.conf.all.disable_ipv6 = 1" /etc/sysctl.conf; then
  info "Disabling IPv6 ..."
  echo "net.ipv6.conf.all.disable_ipv6 = 1" >> /etc/sysctl.conf
fi

echo -e ""
info "------------------------------------------------------------"
info "Gateway Server Install Script Finished"
info "Sentinel account password: ${SENTINEL_PASSWORD}"
info "Reboot is required!"
info "------------------------------------------------------------"
echo -e ""
