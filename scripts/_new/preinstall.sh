#!/usr/bin/env bash



NTP_SERVERS="tik.cesnet.cz tak.cesnet.cz"
NEW_TIMEZONE="Europe/Prague"
NEW_LOCALE="cs_CZ.UTF-8"
NEW_LOCALE_GEN="cs_CZ.UTF-8 UTF-8"

DEFAULT_USER=pi
NEW_USER=sentinel
NEW_PASS=password
ARCH=$(uname -m)
MAC_ADDRESS=$(cat /sys/class/net/eth0/address | sed 's/://g' | tr [:lower:] [:upper:])
NEW_HOSTNAME="SENTINEL-UNIT-${MAC_ADDRESS}"



# ================================================================
# Sentinel Prerequisites Install Script
# ================================================================

function info { echo -e "\e[32m[info] $*\e[39m"; }
function warn  { echo -e "\e[33m[warn] $*\e[39m"; }
function error { echo -e "\e[31m[error] $*\e[39m"; exit 1; }

info "------------------------------------------------------------"
info "Sentinel - Prerequisites Install Script"
info "------------------------------------------------------------"
if [ $(id -u) -ne 0 ]; then
    error "Please run this script with root permissions"
fi
warn "If you want to abort, hit ctrl+c within 10 seconds..."
sleep 10

info "Starting Prerequisites Install Script"



# Generate hardware options
# case $ARCH in
#     "i386" | "i686")
#         MACHINE=${MACHINE:=qemux86}
#         HASSIO_DOCKER="$DOCKER_REPO/i386-hassio-supervisor"
#     ;;
#     "x86_64")
#         MACHINE=${MACHINE:=qemux86-64}
#         HASSIO_DOCKER="$DOCKER_REPO/amd64-hassio-supervisor"
#     ;;
#     "arm" |"armv6l")
#         if [ -z $MACHINE ]; then
#             error "Please set machine for $ARCH"
#         fi
#         HASSIO_DOCKER="$DOCKER_REPO/armhf-hassio-supervisor"
#     ;;
#     "armv7l")
#         if [ -z $MACHINE ]; then
#             error "Please set machine for $ARCH"
#         fi
#         HASSIO_DOCKER="$DOCKER_REPO/armv7-hassio-supervisor"
#     ;;
#     "aarch64")
#         if [ -z $MACHINE ]; then
#             error "Please set machine for $ARCH"
#         fi
#         HASSIO_DOCKER="$DOCKER_REPO/aarch64-hassio-supervisor"
#     ;;
#     *)
#         error "$ARCH unknown!"
#     ;;
# esac

# DEFAULT_USER=$(whoami)



# remove motd
info "Removing default MOTD ..."
rm /etc/motd

            # disable wifi # TODO - if wifi is present
            # info "Disabling WiFi ..."
            # rfkill unblock wifi
            # ifconfig wlan0 down

            # disable bluetooth # TODO - if bluetooth is present
            # info "Disabling Bluetooth ..."
            # systemctl disable hciuart.service
            # systemctl disable bluealsa.service
            # systemctl disable bluetooth.service

# change timezone
info "Changing timezone ..."
timedatectl set-timezone ${NEW_TIMEZONE}

info "Changing RTC into UTC zone ..."
timedatectl set-local-rtc 0 # timedatectl set-local-rtc 1

# set NTP
info "Setting up new NTP configuration ..."
rm /etc/systemd/timesyncd.conf
touch /etc/systemd/timesyncd.conf
chown ${DEFAULT_USER}:${DEFAULT_USER} /etc/systemd/timesyncd.conf
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

# change pi user password
info "Changing default user password ..."
echo -e "${NEW_PASS}\n${NEW_PASS}\n" | passwd ${DEFAULT_USER}

# create sentinel user
info "Creating user sentinel ..."
egrep "^${NEW_USER}" /etc/passwd >/dev/null
if [ $? -eq 0 ]; then
  warn "User ${NEW_USER} already exists"
else
  useradd -u 1111 -m ${NEW_USER}
  info "Changing user sentinel password ..."
  echo -e "${NEW_PASS}\n${NEW_PASS}\n" | passwd ${NEW_USER}
  info "Setting permissions for user sentinel ..."
  usermod -aG adm,dialout,cdrom,sudo,audio,video,plugdev,games,users,input,netdev,spi,i2c,gpio ${NEW_USER}
fi

# edit sentinel user bash profile
chown ${DEFAULT_USER}:${DEFAULT_USER} /home/${NEW_USER}/.bash_profile
{
echo ''
echo 'let upSeconds="$(/usr/bin/cut -d. -f1 /proc/uptime)"'
echo 'let secs=$((${upSeconds}%60))'
echo 'let mins=$((${upSeconds}/60%60))'
echo 'let hours=$((${upSeconds}/3600%24))'
echo 'let days=$((${upSeconds}/86400))'
echo 'UPTIME=`printf "%d days, %02dh %02dm %02ds" "$days" "$hours" "$mins" "$secs"`'
echo ''
echo 'read one five fifteen rest < /proc/loadavg'
echo ''
echo 'echo "'
echo '$(tput setaf 2)Date..................:$(tput setaf 1) `date +"%A, %d. %B %Y, %T %Z"`'
echo '$(tput setaf 2)System................:$(tput setaf 1) `uname -srmo`'
echo '$(tput setaf 2)Uptime................:$(tput setaf 1) ${UPTIME}'
echo '$(tput setaf 2)Memory................:$(tput setaf 1) Total: `free -m | grep Mem | awk '\''{print $2}'\''`MB | Used: `free -m | grep Mem | awk '\''{print $3}'\''`MB | Free: `free -m | grep Mem | awk '\''{print $4}'\''`MB '
echo '$(tput setaf 2)Disk Usage............:$(tput setaf 1) Total: `df -h ~ | grep /dev/root | awk '\''{print $2}'\''`B | Used: `df -h ~ | grep /dev/root | awk '\''{print $3}'\''`B | Free: `df -h ~ | grep /dev/root | awk '\''{print $4}'\''`B | Usage: `df -h ~ | grep /dev/root | awk '\''{print $5}'\''`'
echo '$(tput setaf 2)Load Averages.........:$(tput setaf 1) ${one}, ${five}, ${fifteen} (1, 5, 15 min)'
echo '$(tput setaf 2)Running Processes.....:$(tput setaf 1) `ps ax | wc -l | tr -d " "`'
echo '$(tput setaf 2)IP Address............:$(tput setaf 1) `hostname -I | /usr/bin/cut -d " " -f 1`'
echo '$(tput setaf 2)Default Gateway.......:$(tput setaf 1) `/sbin/ip route | awk '\''/default/ {print $3}'\''`'
echo '$(tput setaf 2)DNS Servers...........:$(tput setaf 1) `cat /etc/resolv.conf | grep '\''nameserver'\'' | awk '\''{ print $2 }'\'' ORS='\'' '\''`'
echo '$(tput setaf 2)MAC Address...........:$(tput setaf 1) `cat /sys/class/net/eth0/address | sed '\''s/:/-/g'\'' | tr [:lower:] [:upper:]`'
echo '$(tput setaf 2)Hostname..............:$(tput setaf 1) `hostname`'
echo '$(tput setaf 2)Temperature...........:$(tput setaf 1) `/opt/vc/bin/vcgencmd measure_temp | cut -c "6-9"`ºC'
echo '$(tput sgr0)"'
echo ''
echo '# if running bash'
echo 'if [ -n "$BASH_VERSION" ]; then'
echo '    # include .bashrc if it exists'
echo '    if [ -f "$HOME/.bashrc" ]; then'
echo '        . "$HOME/.bashrc"'
echo '    fi'
echo 'fi'
echo ''
echo '# set PATH so it includes user'\''s private bin if it exists'
echo 'if [ -d "$HOME/bin" ] ; then'
echo '    PATH="$HOME/bin:$PATH"'
echo 'fi'
echo ''
echo '# set PATH so it includes user'\''s private bin if it exists'
echo 'if [ -d "$HOME/.local/bin" ] ; then'
echo '    PATH="$HOME/.local/bin:$PATH"'
echo 'fi'
echo ''
} >> /home/${NEW_USER}/.bash_profile
chown root:root /home/${NEW_USER}/.bash_profile

# install packages
info "Installing needed packages ..."
apt install -y wget curl git
apt install -y ca-certificates software-properties-common apt-transport-https gnupg lsb-release
apt install -y libffi-dev libssl-dev
apt install -y python3 python3-pip

# install docker
info "Installing docker ..."
curl -sL https://get.docker.com | sh
info "Adding user sentinel into docker group ..."
usermod -aG docker ${NEW_USER}

# install docker compose
info "Installing docker compose ..."
pip3 install docker-compose

# enable docker on start
info "Enabling docker service ..."
systemctl enable docker

# install nodejs
info "Installing nodejs ..."
curl -sL https://deb.nodesource.com/setup_lts.x | bash -
apt install -y nodejs

# apt update
info "Running apt update ..."
apt update -y

# apt upgrade
info "Running apt upgrade ..."
apt upgrade -y

# apt autoremove
info "Running apt autoremove ..."
apt autoremove -y

# set new hostname
info "Setting new hostname ..."
rm /etc/hostname
touch /etc/hostname
chown ${DEFAULT_USER}:${DEFAULT_USER} /etc/hostname
{
echo ${NEW_HOSTNAME}
} >> /etc/hostname
chown root:root /etc/hostname

rm /etc/hosts
touch /etc/hosts
chown ${DEFAULT_USER}:${DEFAULT_USER} /etc/hosts
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

# set new locale
info "Changing locale ..."
rm /etc/locale.gen
touch /etc/locale.gen
chown ${DEFAULT_USER}:${DEFAULT_USER} /etc/locale.gen
{
echo 'en_GB.UTF-8 UTF-8'
echo ${NEW_LOCALE_GEN}
} >> /etc/locale.gen
chown root:root /etc/locale.gen

locale-gen
update-locale LANG=${NEW_LOCALE}

# disable ipv6
info "Disabling IPv6 ..."
touch /etc/sysctl.d/disable-ipv6.conf
chown ${DEFAULT_USER}:${DEFAULT_USER} /etc/sysctl.d/disable-ipv6.conf
{
echo 'net.ipv6.conf.all.disable_ipv6=1'
# echo 'net.ipv6.conf.default.disable_ipv6=1'
# echo 'net.ipv6.conf.lo.disable_ipv6=1'
} >> /etc/sysctl.d/disable-ipv6.conf
chown root:root /etc/sysctl.d/disable-ipv6.conf

sysctl -p -f /etc/sysctl.d/disable-ipv6.conf



info "------------------------------------------------------------"
info "Prerequisites Install Script Finished"
info "New user credentials:  ${NEW_USER}  /  ${NEW_PASS}"
info "Reboot is required!"
info "Use newly created account!"
info "Then continue with other script depending on module"
info "------------------------------------------------------------"
