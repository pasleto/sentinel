<h1 align="center">
  <img src="./documents/logos/readme_logo.png" alt="Logo"/>
</h1>
&nbsp;
<h2 align="center">
  Management and security system for small and midsize businesses
</h2>
&nbsp;
<h3 align="center">
  Prerequisites
</h3>
<p align="center">
  Linux server is needed to run backend services.
  <br>
  Cloud ready.
</p>
&nbsp;
<h3 align="center">
  Server Application Supported Platforms
</h3>
&nbsp;
<p align="center">
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/Debian-✔️-666?style=for-the-badge" alt="Debian"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/Ubuntu-✔️-666?style=for-the-badge" alt="Ubuntu"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/docker-❌-666?style=for-the-badge" alt="docker"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/amd64-✔️-666?style=for-the-badge" alt="amd64"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/arm64-✔️-666?style=for-the-badge" alt="arm64"/>
</p>
&nbsp;
<h3 align="center">
  Used Technologies
</h3>
&nbsp;
<p align="center">
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/Flutter-555?style=for-the-badge&logo=flutter&logoColor=white" alt="Flutter"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/Dart-555?style=for-the-badge&logo=dart&logoColor=white" alt="Dart"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/NodeJS-555?style=for-the-badge&logo=nodedotjs&logoColor=white" alt="NodeJS"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/Javascript-555?style=for-the-badge&logo=javascript&logoColor=white" alt="Javascript"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/C++-555?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/MongoDB-555?style=for-the-badge&logo=mongodb&logoColor=white" alt="MongoDB"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/Bash-555?style=for-the-badge&logo=gnubash&logoColor=white" alt="Bash"/>
</p>
&nbsp;
<h3 align="center">
  Client Application Supported Platforms
</h3>
&nbsp;
<p align="center">
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/WEB-❌-666?style=for-the-badge&logo=googlechrome&logoColor=white" alt="web"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/WINDOWS-❌-666?style=for-the-badge&logo=windows&logoColor=white" alt="windows"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/MACOS-❌-666?style=for-the-badge&logo=apple&logoColor=white" alt="macos"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/LINUX-❌-666?style=for-the-badge&logo=linux&logoColor=white" alt="linux"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/IOS-❌-666?style=for-the-badge&logo=apple&logoColor=white" alt="ios"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/ANDROID-❌-666?style=for-the-badge&logo=android&logoColor=white" alt="android"/>
</p>
&nbsp;

## Project Structure
```
.
├── documents
│   ├── logos                                  # provided images
│   └── manuals                                # provided documentation
├── install_scripts                            # provided installation scripts
│   ├── linux
│   └── windows
├── sentinel_client                            # client application
├── sentinel_gateway                           # server application
├── sentinel_units                             # sentinel units
│   ├── sentinel_access_control                # access control unit
│   ├── sentinel_storage_terminal              # storage terminal unit
│   └── ...
└── ...
```

## Gateway installation
```
! Debian 10 / Debian 11 / Ubuntu 20.04 LTS / Ubuntu 22.04 LTS
! x86_64 / aarch64 (armv8.2-a or later)

▸ curl -sL https://github.com/pasleto/sentinel/raw/main/install_scripts/linux/server_gateway.sh | sudo bash -

! During the installation, you will be prompted to fill these information:

  ● Hostname                                     # Hostname of server
  ● Domain name                                  # Domain name of server
    • Active Directory domain join               # Y/N - if server should be connected to Active Directory
      · Administrator UPN                        # User principal name of Active Directory administrator
      · Administrator password                   #  Password for Active Directory administrator
      · Local admin logon group                  # Active Directory group name to enable logon to server
    • Cloud deployment                           # Y/N - if SSl certificate should be generated
      · List of domains                          # List of domains to generate SSL certificate for
      · Email address                            # Email  address for Let's Encrypt

! If deploying in local network only, default self signed certificate will be used. You need to provide your own certificate (fullchain.pem & privkey.pem) into /opt/sentinel/application/certificates directory.
```
## Raspberry Pi based units installation process
```
! Debian 10 / Debian 11 / Ubuntu 20.04 LTS / Ubuntu 22.04 LTS
! Raspberry Pi 4B recommended

● Storage Terminal Unit
▸ curl -sL https://github.com/pasleto/sentinel/raw/main/install_scripts/linux/storage_terminal.sh | sudo bash -

●
▸ TODO
```
## ESP based units installation process
```
! Parts list is provided
! Build instructions are provided
! Firmware builds are provided

● Access Control Unit


 - TODO
```
## Android based units installation process
```
! Android 7 and above is recommended
! Parts list is provided
! Instructions are provided
! Android application builds are provided

 - TODO
```

&nbsp;
<p align="center">
. . . . . TODO . . . . .
</p>

&nbsp;
<p align="center">
  &nbsp;
  <img src="https://img.shields.io/badge/Developed_By-Tomas_Pasler-blue.svg?style=for-the-badge" alt="pasleto"/>
  &nbsp;
  <img src="https://img.shields.io/badge/Developed_For-Personal_Use-blue.svg?style=for-the-badge" alt="usage"/>
</p>
&nbsp;
<p align="center">
  Copyright &copy; 2022 Tomáš Pásler
</p>
<p align="center">
  <a href="https://github.com/pasleto" target="_blank" rel="noopener noreferrer">Github</a> &nbsp; | &nbsp; <a href="https://linkedin.com/in/tomas-pasler" target="_blank" rel="noopener noreferrer">Linkedin</a> &nbsp; | &nbsp; <a href="https://pasleto.eu" target="_blank" rel="noopener noreferrer">Website</a>
</p>
&nbsp;
<hr>
