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
  <img height="25" src="https://img.shields.io/badge/Ubuntu-❌-666?style=for-the-badge" alt="Ubuntu"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/docker-❌-666?style=for-the-badge" alt="docker"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/amd64-✔️-666?style=for-the-badge" alt="amd64"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/armv7-✔️-666?style=for-the-badge" alt="armv7"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/armv8-✔️-666?style=for-the-badge" alt="armv8"/>
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
│   ├── logos                                       # provided images
│   └── manuals                                     # provided documentation
├── install_scripts                                 # provided installation scripts
│   ├── debian
│   ├── ubuntu
│   └── windows
├── sentinel_client                                 # client application
├── sentinel_gateway                                # server application
│   ├── certificates                                # folder to put your https certificates in
│   ├── client                                      # client web application root folder
│   ├── files                             
│   │   ├── network_share                           # network SMB share root folder
│   │   └── web_share                               # web share root folder
│   ├── logs                                        # server logs root folder
│   ├── server                                      # source files root folder
│   └── ...
├── sentinel_units                                  # sentinel units
│   ├── sentinel_access_control                     # access control unit
│   ├── sentinel_storage_terminal                   # storage terminal unit
│   │   ├── sentinel_storage_terminal_backend
│   │   └── sentinel_storage_terminal_frontend
│   └── ...
└── ...
```

## Gateway installation process (linux - recommended way)
```
! Raspbian Lite is recommended - ARM CPUs
! Debian 11 Server is recommended - x64 CPUs
! Installation scripts are provided

 - TODO
```
## Gateway installation process (windows - alternative way)
```
! Windows Server 2019 and above is recommended
! MongoDB needs to be installed
! NodeJS needs to be installed
! Installation scripts are not provided

 - TODO
```
## ESP based units installation process
```
! Parts list is provided
! Build instructions are provided
! Firmware builds are provided

 - TODO
```
## Raspberry Pi based units installation process
```
! Raspbian Lite is recommended
! Parts list is provided
! Build instructions are provided
! Installation scripts are provided

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
