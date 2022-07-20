#!/usr/bin/env bash

# ================================================================
# Sentinel Storage Terminal Unit Install Script
# ================================================================

SENTINEL_USER=sentinel
HOSTNAME_PREFIX="STL-ST"
MAC_ADDRESS=$(cat /sys/class/net/$(ip route show default | awk '/default/ {print $5}')/address | sed 's/://g' | tr [:lower:] [:upper:])
NEW_HOSTNAME="${HOSTNAME_PREFIX}-${MAC_ADDRESS}"

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Storage Terminal Unit Install Script"
info "------------------------------------------------------------"
# if [ $(uname -m) -ne 'aarch64' ]; then
#   error "Please use 64bit architecture"
# fi
if [ $(logname) != ${SENTINEL_USER} ]; then 
  error "Please login as user sentinel to run this script"
fi
if [ $(id -u) -ne 0 ]; then
  error "Please run this script with sudo"
fi
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

echo -e ""
info "Starting Storage Terminal Unit Install Script"
warn "Standard output is sent to /dev/null"

info "Creating application paths ..."
mkdir -p /opt/sentinel/frontend
mkdir -p /opt/sentinel/backend
sleep 5

info "Setting permissions for user sentinel ..."
usermod -aG render ${SENTINEL_USER}
# chown -R ${SENTINEL_USER}:${SENTINEL_USER} /opt/sentinel
sleep 5

info "Downloading applications ..." #TODO - update url download path
curl -sL 'https://files.pasleto.eu/sentinel/deploy/sentinel-units/linux/sentinel-storage-terminal/latest.zip' -o /opt/sentinel/install/sentinel-storage-terminal.zip
unzip /opt/sentinel/install/sentinel-storage-terminal.zip
sleep 5

info "Installing needed packages ..."
apt-get install -y cmake libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libdrm-dev libgbm-dev ttf-mscorefonts-installer fontconfig libsystemd-dev libinput-dev libudev-dev  libxkbcommon-dev 1>/dev/null
sleep 5
apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-ugly gstreamer1.0-plugins-bad gstreamer1.0-libav 1>/dev/null
sleep 5

info "Updating fonts ..."
fc-cache
sleep 5

# ? above works on both

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
cp -r /opt/sentinel/install/sentinel-storage-terminal/backend/. /opt/sentinel/backend/
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
systemctl enable sentinel-frontend.service
systemctl start sentinel-frontend.service
sleep 5

info "Creating backend service ..." # TODO - nodejs application
echo '' >> /etc/systemd/system/sentinel-backend.service # touch /etc/systemd/system/sentinel-backend.service
echo '[Unit]' >> /etc/systemd/system/sentinel-backend.service
echo 'Sentinel Backend Service' >> /etc/systemd/system/sentinel-backend.service
echo 'After=network.target' >> /etc/systemd/system/sentinel-backend.service
echo '' >> /etc/systemd/system/sentinel-backend.service
echo '[Service]' >> /etc/systemd/system/sentinel-backend.service
echo 'Type=Simple' >> /etc/systemd/system/sentinel-backend.service
echo 'User='${SENTINEL_USER} >> /etc/systemd/system/sentinel-backend.service
echo 'Restart=on-failure' >> /etc/systemd/system/sentinel-backend.service
# echo 'WorkingDirectory=/opt/sentinel/backend' >> /etc/systemd/system/sentinel-backend.service
# echo 'ExecStart=npm start' >> /etc/systemd/system/sentinel-backend.service # /usr/bin/npm
echo 'ExecStart=cross-env NODE_ENV=production node /opt/sentinel/backend/index.js' >> /etc/systemd/system/sentinel-backend.service # /usr/bin/node
echo '' >> /etc/systemd/system/sentinel-backend.service
echo '[Install]' >> /etc/systemd/system/sentinel-backend.service
echo 'WantedBy=multi-user.target' >> /etc/systemd/system/sentinel-backend.service
sleep 5

info "Enabling and starting backend service ..."
systemctl enable sentinel-backend.service
systemctl start sentinel-backend.service
sleep 5


# TODO - check if debian and raspberry pi at top, others will not be supported

if [ -f "/etc/rpi-issue" ]; then # this is raspberry pi os
  info "Changing /boot/config.txt ..."
  echo "hdmi_group=2" >> /boot/config.txt
  echo "hdmi_mode=82" >> /boot/config.txt
  echo "hdmi_cvt 1920 1080 60 6 0 0 0" >> /boot/config.txt
  echo "hdmi_drive=2" >> /boot/config.txt
  sleep 5
fi



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

echo -e ""
info "------------------------------------------------------------"
info " Storage Terminal Unit Install Script Finished"
info " Reboot is required!"
info "------------------------------------------------------------"
echo -e ""
