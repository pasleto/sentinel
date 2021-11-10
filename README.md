<h1 align="center">
  <img src="./docs/images/readme_logo.png" alt="Logo"/>
</h1>
&nbsp;
<p align="center">
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/amd64-yes-blue.svg?style=for-the-badge" alt="amd64"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/docker-yes-blue.svg?style=for-the-badge" alt="docker"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/armv7-yes-blue.svg?style=for-the-badge" alt="armv7"/>
</p>
&nbsp;
<h2 align="center">
  Management and security system for small and midsize businesses
</h2>
&nbsp;
<h3 align="center">
  Prerequisites
</h3>
<p align="center">
  Depends on use case, in general one linux server is needed to run backend services.
  <br>
  Windows server can be used aswell, but installation is not automated.
</p>
&nbsp;
<h3 align="center">
  Client App Supported Systems
</h3>
&nbsp;
<p align="center">
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/WEB-YES-darkgreen.svg?style=for-the-badge" alt="web"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/WINDOWS-YES-darkgreen.svg?style=for-the-badge" alt="windows"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/MACOS-NO-red.svg?style=for-the-badge" alt="macos"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/LINUX-NO-red.svg?style=for-the-badge" alt="linux"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/IOS-NO-red.svg?style=for-the-badge" alt="ios"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/ANDROID-NO-red.svg?style=for-the-badge" alt="android"/>
</p>
&nbsp;

## Project Structure
```
- scripts                             -- installation scripts for linux systems
- sentinel_client                     -- client app flutter project source code
- sentinel_server                     -- client app flutter project source code
  - certificates                      -- folder to put your https certificates in
  - client                            -- client web application root folder
  - files
    - network_share                   -- network SMB share root folder
    - web_share                       -- web share root folder
  - logs                              -- server logs root folder
  - server                            -- server app nodejs source code
- sentinel_units                      -- sub folders for each unit module
  - sentinel_access_unit              -- access unit nodejs project
  - sentinel_attendance_unit          -- attendance unit nodejs project
```

## Server installation process (linux - recommended way)
```
! Raspbian Lite is recommended - ARM CPUs
! Debian Server is recommended - x64 CPUs
! Installation scripts are provided

XXX
```
## Server installation process (windows - alternative way)
```
! Windows Server 2019 and above is recommended
! MongoDB Database Server is need to be installed
! NodeJS need to be installed
! Installation scripts are not provided

XXX
```
## Unit modules installation process
```
! Raspbian Lite running on Raspberry Pi (or other similar board)
! Installation scripts are provided

XXX
```

<br>.
<br>.
<br>.
<br>.
<br>.

&nbsp;
<p align="center">
  &nbsp;
  <img src="https://img.shields.io/badge/Developed_By-Tomas_Pasler-blue.svg?style=for-the-badge" alt="pasleto"/>
  &nbsp;
  <img src="https://img.shields.io/badge/Developed_For-Personal_Use-ffa500.svg?style=for-the-badge" alt="usage"/>
</p>
&nbsp;
<p align="center">
  Copyright &copy; 2021 Tomáš Pásler
</p>
<p align="center">
  <a href="https://github.com/pasleto" target="_blank" rel="noopener noreferrer">Github</a> &nbsp; | &nbsp; <a href="https://linkedin.com/in/tomas-pasler" target="_blank" rel="noopener noreferrer">Linkedin</a> &nbsp; | &nbsp; <a href="https://pasleto.eu" target="_blank" rel="noopener noreferrer">Website</a>
</p>
&nbsp;
<hr>
