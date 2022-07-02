#!/usr/bin/env bash



NTP_SERVERS="tik.cesnet.cz tak.cesnet.cz" # TODO - make as prompt or config file
NEW_TIMEZONE="Europe/Prague"



# ================================================================
# Sentinel Prerequisites Install Script
# ================================================================

# ARCH=$(uname -m)
# ARCH=$(dpkg --print-architecture)
CURRENT_HOSTNAME=$(cat /etc/hostname | tr -d ' \t\n\r')
CURRENT_USERNAME=$(logname)
SENTINEL_USERNAME=sentinel
SENTINEL_PASSWORD=password
HOSTNAME_PREFIX="STL"
MAC_ADDRESS=$(cat /sys/class/net/$(ip route show default | awk '/default/ {print $5}')/address | sed 's/://g' | tr [:lower:] [:upper:])
NEW_HOSTNAME="${HOSTNAME_PREFIX}-${MAC_ADDRESS}"

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Prerequisites Install Script"
info "------------------------------------------------------------"
if [ $(id -u) -ne 0 ]; then
  error "Please run this script with sudo"
fi
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

echo -e ""
info "Starting Prerequisites Install Script"
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

# info "Changing default user password ..." # ? - maybe not doing this
# echo ${CURRENT_USERNAME}:${SENTINEL_PASSWORD} | chpasswd
# sleep 5

if [ -f "/etc/motd" ]; then
  info "Removing MOTD ..."
  rm /etc/motd
fi
sleep 5

info "Changing timezone ..."
timedatectl set-timezone ${NEW_TIMEZONE}
sleep 5

info "Changing RTC into UTC zone ..."
timedatectl set-local-rtc 0 # timedatectl set-local-rtc 1
sleep 5

info "Setting up new NTP configuration ..." # ? maybe use sed instead
rm /etc/systemd/timesyncd.conf
touch /etc/systemd/timesyncd.conf
chown ${CURRENT_USERNAME}:${CURRENT_USERNAME} /etc/systemd/timesyncd.conf
{
echo '[Time]'
echo 'NTP='${NTP_SERVERS}
echo 'FallbackNTP=0.debian.pool.ntp.org 1.debian.pool.ntp.org 2.debian.pool.ntp.org 3.debian.pool.ntp.org'
echo 'RootDistanceMaxSec=5'
echo 'PollIntervalMinSec=32'
echo 'PollIntervalMaxSec=2048'
} >> /etc/systemd/timesyncd.conf
chown root:root /etc/systemd/timesyncd.conf
timedatectl set-ntp true
systemctl daemon-reload
systemctl restart systemd-timesyncd
sleep 5

info "Creating user sentinel ..."
egrep "^${SENTINEL_USERNAME}" /etc/passwd >/dev/null
if [ $? -eq 0 ]; then
  warn "User ${SENTINEL_USERNAME} already exists"
else
  useradd -u 1111 -m ${SENTINEL_USERNAME}
  info "Changing user sentinel password ..."
  echo ${SENTINEL_USERNAME}:${SENTINEL_PASSWORD} | chpasswd
fi
sleep 5

info "Setting login dashboard for user sentinel ..."
echo '' >> /home/${SENTINEL_USERNAME}/.profile
echo 'echo "' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)Date......................:$(tput setaf 1) `date +"%A, %d. %B %Y, %T %Z"`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)System....................:$(tput setaf 1) `uname -srmo`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)Uptime....................:$(tput setaf 1) `printf "%d days, %02dh %02dm %02ds" "$(($(cut -d. -f1 /proc/uptime)/86400))" "$(($(cut -d. -f1 /proc/uptime)/3600%24))" "$(($(cut -d. -f1 /proc/uptime)/60%60))" "$(($(cut -d. -f1 /proc/uptime)%60))"`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)Memory....................:$(tput setaf 1) Total: `free -m | grep Mem | awk '\''{print $2}'\''`MB | Used: `free -m | grep Mem | awk '\''{print $3}'\''`MB | Free: `free -m | grep Mem | awk '\''{print $4}'\''`MB' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)Disk Usage................:$(tput setaf 1) Total: `df -h ~ | grep /dev/root | awk '\''{print $2}'\''`B | Used: `df -h ~ | grep /dev/root | awk '\''{print $3}'\''`B | Free: `df -h ~ | grep /dev/root | awk '\''{print $4}'\''`B | Usage: `df -h ~ | grep /dev/root | awk '\''{print $5}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)Load Averages.............:$(tput setaf 1) `cut -c "1-4" /proc/loadavg` (1 min) | `cut -c "6-9" /proc/loadavg` (5 min) | `cut -c "11-14" /proc/loadavg` (15 min)' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)Running Processes.........:$(tput setaf 1) `ps ax | wc -l | tr -d " "`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)IP Address................:$(tput setaf 1) `hostname -I | cut -d " " -f1`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)Default Gateway...........:$(tput setaf 1) `/sbin/ip route | awk '\''/default/ {print $3}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)DNS Servers...............:$(tput setaf 1) `cat /etc/resolv.conf | grep '\''nameserver'\'' | awk '\''{ print $2 }'\'' ORS='\'' '\''`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)MAC Address...............:$(tput setaf 1) `cat /sys/class/net/$(/sbin/ip route | awk '\''/default/ {print $5}'\'')/address | sed '\''s/:/-/g'\'' | tr [:lower:] [:upper:]`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)Default Ethernet Port.....:$(tput setaf 1) `/sbin/ip route | awk '\''/default/ {print $5}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)Hostname..................:$(tput setaf 1) `hostname -f`' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput setaf 2)CPU Temperature...........:$(tput setaf 1) `cat /sys/class/thermal/thermal_zone0/temp | awk '\''{print $1/1000}'\''`ºC' >> /home/${SENTINEL_USERNAME}/.profile
echo '$(tput sgr0)"' >> /home/${SENTINEL_USERNAME}/.profile
echo '' >> /home/${SENTINEL_USERNAME}/.profile
sleep 5
# cat /sys/devices/virtual/thermal/thermal_zone*/temp | awk '{printf " %5.2f °C   " , $1/1000}'
# paste <(cat /sys/class/thermal/thermal_zone*/type) <(cat /sys/class/thermal/thermal_zone*/temp) | column -s $'\t' -t | sed 's/\(.\)..$/.\1°C/'
# echo '$(tput setaf 2)MAC Address...........:$(tput setaf 1) `cat /sys/class/net/eth0/address | sed '\''s/:/-/g'\'' | tr [:lower:] [:upper:]`' >> /home/${SENTINEL_USERNAME}/.profile
# echo '$(tput setaf 2)GPU Temperature.......:$(tput setaf 1) `/usr/bin/vcgencmd measure_temp | cut -c "6-9"`ºC' >> /home/${SENTINEL_USERNAME}/.profile

info "Disabling login message for user sentinel ..."
touch /home/${SENTINEL_USERNAME}/.hushlogin

if [ -f "/etc/rpi-issue" ]; then # this is raspberry pi os

  info "Changing /boot/config.txt ..."
  echo "dtoverlay=disable-wifi" >> /boot/config.txt
  echo "dtoverlay=disable-bt" >> /boot/config.txt
  echo "disable_splash=1" >> /boot/config.txt
  # TODO - enable i2c and spi

  info "Changing /boot/cmdline.txt ..."
  sed -i 's/tty1/tty3/g' /boot/cmdline.txt
  echo " quiet logo.nologo vt.global_cursor_default=0 consoleblank=0" >> /boot/cmdline.txt

  info "Setting permissions for user sentinel ..."
  usermod -aG adm,dialout,cdrom,sudo,audio,video,plugdev,users,input,netdev,spi,i2c,gpio ${SENTINEL_USERNAME}
  # usermod -aG adm,dialout,cdrom,sudo,audio,video,plugdev,users,input,netdev,spi,i2c,gpio,render ${SENTINEL_USERNAME}

else # TODO - this is other linux distro

  # TODO - check for Debian || Ubuntu
  # TODO - maybe handle before hand

  info "Setting permissions for user sentinel ..."
  usermod -aG adm,dialout,cdrom,sudo,audio,video,plugdev,users,netdev,systemd-journal ${SENTINEL_USERNAME} # staff,shadow

fi

info "Installing needed packages ..."
apt-get install -y wget curl git 1>/dev/null
apt-get install -y libffi-dev libssl-dev 1>/dev/null
apt-get install -y python3-dev 1>/dev/null
apt-get install -y python3 python3-pip 1>/dev/null
apt-get install -y ca-certificates software-properties-common apt-transport-https gnupg lsb-release 1>/dev/null
sleep 5

info "Installing docker ..."
curl -sL https://get.docker.com | sh &>/dev/null
sleep 5

info "Adding users into docker group ..."
usermod -aG docker ${SENTINEL_USERNAME}
sleep 5

info "Installing nodejs ..."
curl -sL https://deb.nodesource.com/setup_lts.x | bash - 1>/dev/null
apt-get install -y nodejs 1>/dev/null
sleep 5

info "Installing docker compose ..."
pip3 install docker-compose 1>/dev/null
sleep 5

info "Enabling docker service ..."
systemctl -q enable docker
sleep 5

info "Running apt-get update ..."
apt-get update -y 1>/dev/null
sleep 5

info "Running apt-get upgrade ..."
apt-get upgrade -y 1>/dev/null
sleep 5

info "Running apt-get autoremove ..."
apt-get autoremove -y 1>/dev/null
sleep 5

info "Setting new hostname ..."
echo ${NEW_HOSTNAME} > /etc/hostname
sed -i "s/127.0.1.1.*${CURRENT_HOSTNAME}/127.0.1.1\t${NEW_HOSTNAME}/g" /etc/hosts
hostname ${NEW_HOSTNAME}
sleep 5

info "Disabling IPv6 ..."
echo "net.ipv6.conf.all.disable_ipv6 = 1" >> /etc/sysctl.conf
sleep 5

echo -e ""
info "------------------------------------------------------------"
info " Prerequisites Install Script Finished"
info " Reboot is required!"
info " New user credentials:  ${SENTINEL_USERNAME}  /  ${SENTINEL_PASSWORD}"
info " Use newly created account!"
info " Then continue with another script depending on unit"
info "------------------------------------------------------------"
echo -e ""
