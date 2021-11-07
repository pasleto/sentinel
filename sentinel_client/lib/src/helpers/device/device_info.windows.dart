import 'dart:io';

Map<String, String> get envVars => Platform.environment;
String? userName = envVars['username'];
String? userDomain = envVars['userdomain'];
String? userDnsDomain = envVars['userdnsdomain'];
String? computerName = envVars['computername'];
String? computerSystem = envVars['os'];
String? userAppData = envVars['appdata'];
String? userLocalAppData = envVars['localappdata'];
String? userProfile = envVars['userprofile'];

String getWindowsDomainLogonName() {
  return userDomain.toString() + '\\' + userName.toString();
}

String getWindowsDomainUserName() {
  if (userDnsDomain == null) {
    return userName.toString();
  } else {
    return userName.toString() + '@' + userDnsDomain.toString();
  }
}

String getWindowsUserName() {
  return userName.toString();
}

String getWindowsDomainName() {
  return userDomain.toString();
}

String getWindowsDnsDomainName() {
  return userDnsDomain.toString();
}

String getWindowsComputerName() {
  return computerName.toString();
}

String getWindowsComputerSystem() {
  return computerSystem.toString();
}

String getWindowsUserAppData() {
  return userAppData.toString();
}

String getWindowsUserLocalAppData() {
  return userLocalAppData.toString();
}

String getWindowsUserProfile() {
  return userProfile.toString();
}
