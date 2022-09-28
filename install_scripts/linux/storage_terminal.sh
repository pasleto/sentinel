#!/usr/bin/env bash

# ================================================================
# TODO Sentinel Storage Terminal Unit Install Script
# ================================================================

DISTRO=$(lsb_release -si)
CURRENT_HOSTNAME=$(hostname -s) # $(cat /etc/hostname | tr -d ' \t\n\r')
CURRENT_USERNAME=$(logname)
SENTINEL_USERNAME=sentinel
SENTINEL_PASSWORD=password
HOSTNAME_PREFIX="STL-ST"
MAC_ADDRESS=$(cat /sys/class/net/$(/sbin/ip route | awk '/default/ {print $5}')/address | sed 's/://g' | tr [:lower:] [:upper:]) # $(cat /sys/class/net/$(ip route show default | awk '/default/ {print $5}')/address | sed 's/://g' | tr [:lower:] [:upper:])
NEW_HOSTNAME="${HOSTNAME_PREFIX}-${MAC_ADDRESS}"
NEW_TIMEZONE="UTC" # default value # Europe/Prague
NTP_SERVERS="time.cloudflare.com time.google.com" # default value # tik.cesnet.cz tak.cesnet.cz
FALLBACK_NTP_SERVERS="time.cloudflare.com time.google.com"

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Storage Terminal Unit Install Script"
info "------------------------------------------------------------"
# if [ $(uname -m) != "aarch64" ]; then
#   error "Please use 64bit architecture"
# fi
if [ $(id -u) -ne 0 ]; then
  error "Please run this script with sudo"
fi
warn "OS should be fully up to date and rebooted before running this script!"
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

echo -e ""
info "Starting Storage Terminal Unit Install Script"
warn "Standard output is sent to /dev/null"

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

info "Setting up new NTP configuration ..."
sed -i "/Fallback/!s/.*NTP=.*/NTP=${NTP_SERVERS}/g" /etc/systemd/timesyncd.conf
sed -i "/Fallback/s/.*FallbackNTP=.*/FallbackNTP=${FALLBACK_NTP_SERVERS}/g" /etc/systemd/timesyncd.conf
sed -i "s/.*RootDistanceMaxSec=.*/RootDistanceMaxSec=5/g" /etc/systemd/timesyncd.conf
sed -i "s/.*PollIntervalMinSec=.*/PollIntervalMinSec=32/g" /etc/systemd/timesyncd.conf
sed -i "s/.*PollIntervalMaxSec=.*/PollIntervalMaxSec=2048/g" /etc/systemd/timesyncd.conf
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

  info "Changing user sentinel shell to bash ..." # TODO - validate on debian
  usermod -s /bin/bash ${SENTINEL_USERNAME}
  sleep 5

  info "Setting user sentinel login dashboard ..." # ? - disk usage debian - /dev/root | ubuntu - /dev/sda1 (cloud) ?? - /dev/^*
  echo '' >> /home/${SENTINEL_USERNAME}/.profile
  echo 'echo "' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)Date......................:$(tput setaf 1) `date +"%A, %d. %B %Y, %T %Z"`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)System....................:$(tput setaf 1) `uname -srmo`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)Uptime....................:$(tput setaf 1) `printf "%d days, %02dh %02dm %02ds" "$(($(cut -d. -f1 /proc/uptime)/86400))" "$(($(cut -d. -f1 /proc/uptime)/3600%24))" "$(($(cut -d. -f1 /proc/uptime)/60%60))" "$(($(cut -d. -f1 /proc/uptime)%60))"`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)Memory....................:$(tput setaf 1) Total: `free -m | grep Mem | awk '\''{print $2}'\''`MB | Used: `free -m | grep Mem | awk '\''{print $3}'\''`MB | Free: `free -m | grep Mem | awk '\''{print $4}'\''`MB' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)Disk Usage................:$(tput setaf 1) Total: `df -h ~ | grep /dev/^* | awk '\''{print $2}'\''`B | Used: `df -h ~ | grep /dev/^* | awk '\''{print $3}'\''`B | Free: `df -h ~ | grep /dev/^* | awk '\''{print $4}'\''`B | Usage: `df -h ~ | grep /dev/^* | awk '\''{print $5}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)Load Averages.............:$(tput setaf 1) `cut -c "1-4" /proc/loadavg` (1 min) | `cut -c "6-9" /proc/loadavg` (5 min) | `cut -c "11-14" /proc/loadavg` (15 min)' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)Running Processes.........:$(tput setaf 1) `ps ax | wc -l | tr -d " "`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)IP Address................:$(tput setaf 1) `hostname -I | cut -d " " -f1`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)Default Gateway...........:$(tput setaf 1) `/sbin/ip route | awk '\''/default/ {print $3}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)DNS Servers...............:$(tput setaf 1) `resolvectl dns | grep $(/sbin/ip route | awk '\''/default/ {print $5}'\'') | awk '\''{split($0,a,": "); print a[2]}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)MAC Address...............:$(tput setaf 1) `cat /sys/class/net/$(/sbin/ip route | awk '\''/default/ {print $5}'\'')/address | sed '\''s/:/-/g'\'' | tr [:lower:] [:upper:]`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)Default Ethernet Port.....:$(tput setaf 1) `/sbin/ip route | awk '\''/default/ {print $5}'\''`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)Hostname..................:$(tput setaf 1) `hostname -f`' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput setaf 2)CPU Temperature...........:$(tput setaf 1) `cat /sys/class/thermal/thermal_zone0/temp | awk '\''{print $1/1000}'\''`ºC' >> /home/${SENTINEL_USERNAME}/.profile
  echo '$(tput sgr0)"' >> /home/${SENTINEL_USERNAME}/.profile
  echo '' >> /home/${SENTINEL_USERNAME}/.profile
  sleep 5
fi

info "Disabling user sentinel login message ..."
touch /home/${SENTINEL_USERNAME}/.hushlogin
sleep 5

case $DISTRO in
  "Debian")
    if [ -f "/etc/rpi-issue" ]; then
      info "Changing /boot/config.txt ..."
      echo "dtoverlay=disable-wifi" >> /boot/config.txt
      echo "dtoverlay=disable-bt" >> /boot/config.txt
      echo "disable_splash=1" >> /boot/config.txt
      echo "hdmi_group=2" >> /boot/config.txt
      echo "hdmi_mode=82" >> /boot/config.txt
      echo "hdmi_cvt 1920 1080 60 6 0 0 0" >> /boot/config.txt
      echo "hdmi_drive=2" >> /boot/config.txt
      # TODO - enable i2c and spi
      sleep 5

      info "Changing /boot/cmdline.txt ..."
      sed -i "s/tty1/tty3/g" /boot/cmdline.txt
      echo " quiet logo.nologo vt.global_cursor_default=0 consoleblank=0" >> /boot/cmdline.txt
      sleep 5

      info "Setting permissions for user sentinel ..."
      usermod -aG adm,dialout,cdrom,sudo,audio,video,plugdev,users,input,netdev,render,spi,i2c,gpio ${SENTINEL_USERNAME}
    else
      info "Setting permissions for user sentinel ..."
      usermod -aG adm,dialout,cdrom,sudo,audio,video,plugdev,users,netdev,render ${SENTINEL_USERNAME} # staff,shadow
      sleep 5
    fi
  ;;
  "Ubuntu")

    # ? - cat /proc/cpuinfo | grep 'Model' | awk '{ print $3 }'
    # ? -> Raspberry

    # ? - cat /proc/device-tree/model
    # ? -> Raspberry Pi 4 Model B Rev 1.2

    # ? - egrep "BCM(283(5|6|7)|270(8|9)|2711)" /proc/cpuinfo

    # TODO

    if [ -f "/etc/cloud/cloud.cfg" ]; then
      info "Disabling cloud init ..."
      touch /etc/cloud/cloud-init.disabled
    fi

    info "Setting permissions for user sentinel ..."
    usermod -aG adm,dialout,cdrom,sudo,audio,video,plugdev,users,netdev,render ${SENTINEL_USERNAME} # staff,shadow
    sleep 5
  ;;
  *)
    error "$DISTRO is not supported."
  ;;
esac

info "Installing needed packages ..."
apt-get install -y wget curl git 1>/dev/null
apt-get install -y ca-certificates software-properties-common apt-transport-https gnupg lsb-release 1>/dev/null
sleep 5
apt-get install -y cmake libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libdrm-dev libgbm-dev ttf-mscorefonts-installer fontconfig libsystemd-dev libinput-dev libudev-dev  libxkbcommon-dev 1>/dev/null
sleep 5
apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-ugly gstreamer1.0-plugins-bad gstreamer1.0-libav 1>/dev/null
sleep 5

info "Updating fonts ..."
fc-cache
sleep 5

info "Installing nodejs ..."
curl -sL https://deb.nodesource.com/setup_lts.x | bash - 1>/dev/null
apt-get install -y nodejs 1>/dev/null
sleep 5





info "Creating application paths ..."
mkdir -p /opt/sentinel/install
mkdir -p /opt/sentinel/frontend
mkdir -p /opt/sentinel/backend
sleep 5

info "Downloading applications ..." #TODO - update url download path + based on architecture (32 vs 64)
curl -sL 'https://files.pasleto.eu/sentinel/deploy/sentinel-units/linux/sentinel-storage-terminal/latest.zip' -o /opt/sentinel/install/sentinel-storage-terminal.zip
unzip /opt/sentinel/install/sentinel-storage-terminal.zip
sleep 5

info "Installing flutter-pi-engine-binaries ..." # /opt/sentinel/install/sentinel-storage-terminal/flutter-pi-engine-binaries
/opt/sentinel/install/sentinel-storage-terminal/flutter-pi-engine-binaries/install.sh # ? install flutter-pi-engine-binaries
sleep 5

info "Installing flutter-pi ..." # /opt/sentinel/install/sentinel-storage-terminal/flutter-pi
mdkir /opt/sentinel/install/sentinel-storage-terminal/flutter-pi/build
pushd /opt/sentinel/install/sentinel-storage-terminal/flutter-pi/build # ? install flutter-pi
cmake ..
make -j`nproc`
popd
sleep 5

info "Installing applications ..." # TODO - update url download path
cp -r /opt/sentinel/install/sentinel-storage-terminal/frontend/. /opt/sentinel/frontend/
# cp -r /opt/sentinel/install/sentinel-storage-terminal/backend/. /opt/sentinel/backend/
# TODO npm install backend
sleep 5

info "Creating frontend service ..."
echo '' >> /etc/systemd/system/sentinel-frontend.service # touch /etc/systemd/system/sentinel-frontend.service
echo '[Unit]' >> /etc/systemd/system/sentinel-frontend.service
echo 'Sentinel Frontend Service' >> /etc/systemd/system/sentinel-frontend.service
echo '' >> /etc/systemd/system/sentinel-frontend.service
echo '[Service]' >> /etc/systemd/system/sentinel-frontend.service
echo 'Type=Simple' >> /etc/systemd/system/sentinel-frontend.service
echo 'User='${SENTINEL_USER} >> /etc/systemd/system/sentinel-frontend.service
echo 'Restart=on-failure' >> /etc/systemd/system/sentinel-frontend.service
echo 'ExecStart=flutter-pi --release /opt/sentinel/frontend' >> /etc/systemd/system/sentinel-frontend.service
echo '' >> /etc/systemd/system/sentinel-frontend.service
echo '[Install]' >> /etc/systemd/system/sentinel-frontend.service
echo 'WantedBy=multi-user.target' >> /etc/systemd/system/sentinel-frontend.service
sleep 5

info "Enabling and starting frontend service ..."
systemctl -q enable sentinel-frontend.service
systemctl -q start sentinel-frontend.service
sleep 5

# info "Creating backend service ..." # TODO - nodejs application
# echo '' >> /etc/systemd/system/sentinel-backend.service # touch /etc/systemd/system/sentinel-backend.service
# echo '[Unit]' >> /etc/systemd/system/sentinel-backend.service
# echo 'Sentinel Backend Service' >> /etc/systemd/system/sentinel-backend.service
# echo 'After=network.target' >> /etc/systemd/system/sentinel-backend.service
# echo '' >> /etc/systemd/system/sentinel-backend.service
# echo '[Service]' >> /etc/systemd/system/sentinel-backend.service
# echo 'Type=Simple' >> /etc/systemd/system/sentinel-backend.service
# echo 'User='${SENTINEL_USER} >> /etc/systemd/system/sentinel-backend.service
# echo 'Restart=on-failure' >> /etc/systemd/system/sentinel-backend.service
# # echo 'WorkingDirectory=/opt/sentinel/backend' >> /etc/systemd/system/sentinel-backend.service
# # echo 'ExecStart=npm start' >> /etc/systemd/system/sentinel-backend.service # /usr/bin/npm
# echo 'ExecStart=cross-env NODE_ENV=production node /opt/sentinel/backend/index.js' >> /etc/systemd/system/sentinel-backend.service # /usr/bin/node
# echo '' >> /etc/systemd/system/sentinel-backend.service
# echo '[Install]' >> /etc/systemd/system/sentinel-backend.service
# echo 'WantedBy=multi-user.target' >> /etc/systemd/system/sentinel-backend.service
# sleep 5

# info "Enabling and starting backend service ..."
# systemctl -q enable sentinel-backend.service
# systemctl -q start sentinel-backend.service
# sleep 5

info "Disabling console service on host ..."
systemctl disable getty@tty1.service
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
sed -i "s/${CURRENT_HOSTNAME}/${NEW_HOSTNAME}/g" /etc/hosts
hostname ${NEW_HOSTNAME}
sleep 5

info "Disabling IPv6 ..."
echo "net.ipv6.conf.all.disable_ipv6 = 1" >> /etc/sysctl.conf
sleep 5

echo -e ""
info "------------------------------------------------------------"
info " Storage Terminal Unit Install Script Finished"
info " Reboot is required!"
info "------------------------------------------------------------"
echo -e ""
