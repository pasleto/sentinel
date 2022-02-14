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
<h3 align="center">
  Management and security system for small and midsize businesses
</h3>
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
</p>
<p align="center">
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/MACOS-NOT_YET-red.svg?style=for-the-badge" alt="macos"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/LINUX-NOT_YET-red.svg?style=for-the-badge" alt="linux"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/IOS-NOT_YET-red.svg?style=for-the-badge" alt="ios"/>
  &nbsp;
  <img height="25" src="https://img.shields.io/badge/ANDROID-NOT_YET-red.svg?style=for-the-badge" alt="android"/>
</p>
&nbsp;

## Project Structure
```
- scripts                               -- installation scripts for linux systems
- sentinel_client                       -- client application
- sentinel_gateway                      -- server application
  - certificates                        -- folder to put your https certificates in
  - client                              -- client web application root folder
  - files                               -- files hosting
    - network_share                     -- network SMB share root folder
    - web_share                         -- web share root folder
  - logs                                -- server logs root folder
  - server                              -- server app nodejs source files
- sentinel_units                        -- sentinel units projects
  - sentinel_access_unit                -- access control unit
  - sentinel_attendance_unit            -- attendance unit
  - sentinel_bus_client                 -- bus commuting unit
  - sentinel_car_unit                   -- car gps monitoring unit
  - sentinel_signage_display_unit       -- signage display unit
  - sentinel_environment_unit           -- environment monitoring unit
  - sentinel_meeting_room_unit          -- meeting room display unit
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
  <img src="https://img.shields.io/badge/Developed_For-Personal_Use-ffa500.svg?style=for-the-badge" alt="usage"/>
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
