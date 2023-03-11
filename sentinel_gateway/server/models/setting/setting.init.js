const settingInitData = [
  { // General
    'scope': 'general',
    'name': 'timezone',
    'friendly_name': 'Timezone',
    'description': 'Active Timezone (default: Etc_UTC)',
    'value': 'Etc_UTC',
    'is_protected': false
  },
  {
    'scope': 'general',
    'name': 'hash_card',
    'friendly_name': 'Card Hash',
    'description': 'Hash for cards hashing (default: auto-generated)',
    'value': '',
    'is_protected': true
  },
  {
    'scope': 'general',
    'name': 'hash_jwt',
    'friendly_name': 'JWT Hash',
    'description': 'Hash for JWT generation (default: auto-generated)',
    'value': '',
    'is_protected': true
  },
  {
    'scope': 'general',
    'name': 'expiration_jwt',
    'friendly_name': 'JWT Expiration',
    'description': 'JWT expiration period (default: 6h)',
    'value': '6h',
    'is_protected': true
  },
  { // Map
    'scope': 'map',
    'name': 'center_lat',
    'friendly_name': 'Map Center Latitude',
    'description': 'Map Center Point - Latitude (default: 0.0)',
    'value': '0.0',
    'is_protected': false
  },
  {
    'scope': 'map',
    'name': 'center_lon',
    'friendly_name': 'Map Center Longitude',
    'description': 'Map Center Point - Longitude (default: 0.0)',
    'value': '0.0',
    'is_protected': false
  },
  {
    'scope': 'map',
    'name': 'center_zoom',
    'friendly_name': 'Map Center Zoom',
    'description': 'Map Center Point - Zoom Level - Value between 2 and 18 (default: 15.0)',
    'value': '15.0',
    'is_protected': false
  },
  { // Proxy
    'scope': 'proxy',
    'name': 'url_string',
    'friendly_name': 'Proxy Server URL String',
    'description': 'Proxy URL - eg. http://[server]:[port] or http://[user]:[password]@[server]:[port] (default: empty)',
    'value': '',
    'is_protected': false
  },
  { // LDAP
    'scope': 'ldap',
    'name': 'use_auth',
    'friendly_name': 'Use Authentication',
    'description': 'If LDAP authentication is enabled (default: false)',
    'value': 'false',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'use_sync',
    'friendly_name': 'Use Synchronization',
    'description': 'If LDAP synchronization is enabled (default: false)',
    'value': 'false',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'host_url',
    'friendly_name': 'LDAP Server Name',
    'description': 'Host URL (default: dc.example.com)',
    'value': 'dc.example.com',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'host_port',
    'friendly_name': 'LDAP Server Port',
    'description': 'Host PORT (default: 389)',
    'value': '389',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'host_ssl',
    'friendly_name': 'Secured Version',
    'description': 'If secured version of LDAP is used (default: false)',
    'value': 'false',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'username',
    'friendly_name': 'Administrator Account Username',
    'description': 'User with access to Active Directory (default: sentinel@example.com)',
    'value': 'sentinel@example.com',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'password',
    'friendly_name': 'Administrator Account Password',
    'description': 'User password (default: password)',
    'value': 'password',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'domain_dns_name',
    'friendly_name': 'Domain DNS Name',
    'description': 'Domain DNS Name (default: example.com)',
    'value': 'example.com',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'domain_netbios_name',
    'friendly_name': 'Domain NetBIOS Name',
    'description': 'Domain NetBIOS Name (default: example)',
    'value': 'example',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'basedn_root',
    'friendly_name': 'Root BaseDN',
    'description': 'Topmost container BaseDN (default: dc=example,dc=com)',
    'value': 'dc=example,dc=com',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'basedn_users',
    'friendly_name': 'Users BaseDN',
    'description': 'Users container BaseDN (default: ou=users,dc=example,dc=com)',
    'value': 'cn=users,dc=example,dc=com',
    'is_protected': false
  },

  {
    'scope': 'ldap',
    'name': 'group_mqtt_dn',
    'friendly_name': 'MQTT Auth Group',
    'description': 'Group for MQTT broker authentication (eg: cn=MQTT,ou=users,dc=example,dc=com)',
    'value': '',
    'is_protected': false
  },

  {
    'scope': 'ldap',
    'name': 'attr_user_cn',
    'friendly_name': 'User Attribute - cn',
    'description': 'User Attribute Mapping - cn (default: cn)',
    'value': 'cn',
    'is_protected': true
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_dn',
    'friendly_name': 'User Attribute - dn',
    'description': 'User Attribute Mapping - dn (default: dn)',
    'value': 'dn',
    'is_protected': true
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_userprincipalname',
    'friendly_name': 'User Attribute - userPrincipalName',
    'description': 'User Attribute Mapping - userPrincipalName (default: userPrincipalName)',
    'value': 'userPrincipalName',
    'is_protected': true
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_samaccountname',
    'friendly_name': 'User Attribute - sAMAccountName',
    'description': 'User Attribute Mapping - sAMAccountName (default: sAMAccountName)',
    'value': 'sAMAccountName',
    'is_protected': true
  },
  // { // TODO
  //   'scope': 'ldap',
  //   'name': 'attr_user_jpegphoto',
  //   'friendly_name': 'User Attribute - jpegPhoto',
  //   'description': 'User Attribute Mapping - jpegPhoto (default: jpegPhoto)',
  //   'value': 'jpegPhoto',
  //   'is_protected': true
  // },
  { // TODO
    'scope': 'ldap',
    'name': 'attr_user_thumbnailphoto',
    'friendly_name': 'User Attribute - thumbnailPhoto',
    'description': 'User Attribute Mapping - thumbnailPhoto (default: thumbnailPhoto)',
    'value': 'thumbnailPhoto',
    'is_protected': true
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_description',
    'friendly_name': 'User Attribute - description',
    'description': 'User Attribute Mapping - description (default: description)',
    'value': 'description',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_mail',
    'friendly_name': 'User Attribute - mail',
    'description': 'User Attribute Mapping - mail (default: mail)',
    'value': 'mail',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_mobile',
    'friendly_name': 'User Attribute - mobile',
    'description': 'User Attribute Mapping - mobile (default: mobile)',
    'value': 'mobile',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_telephonenumber',
    'friendly_name': 'User Attribute - telephoneNumber',
    'description': 'User Attribute Mapping - telephoneNumber (default: telephoneNumber)',
    'value': 'telephoneNumber',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_title',
    'friendly_name': 'User Attribute - title',
    'description': 'User Attribute Mapping - title (default: title)',
    'value': 'title',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_objectsid',
    'friendly_name': 'User Attribute - objectSid',
    'description': 'User Attribute Mapping - objectSid (default: objectSid)',
    'value': 'objectSid',
    'is_protected': true
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_departmentnumber',
    'friendly_name': 'User Attribute - departmentNumber',
    'description': 'User Attribute Mapping - departmentNumber (default: departmentNumber)',
    'value': 'departmentNumber',
    'is_protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_manager',
    'friendly_name': 'User Attribute - manager',
    'description': 'User Attribute Mapping - manager (default: manager)',
    'value': 'manager',
    'is_protected': true
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_givenname',
    'friendly_name': 'User Attribute - givenName',
    'description': 'User Attribute Mapping - givenName (default: givenName)',
    'value': 'givenName',
    'is_protected': true
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_sn',
    'friendly_name': 'User Attribute - sn',
    'description': 'User Attribute Mapping - sn (default: sn)',
    'value': 'sn',
    'is_protected': true
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_useraccountcontrol',
    'friendly_name': 'User Attribute - userAccountControl',
    'description': 'User Attribute Mapping - userAccountControl (default: userAccountControl)',
    'value': 'userAccountControl',
    'is_protected': true
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_employeenumber',
    'friendly_name': 'User Attribute - employeeNumber',
    'description': 'User Attribute Mapping - employeeNumber (default: employeeNumber)',
    'value': 'employeeNumber',
    'is_protected': false
  },
  { // SMTP
    'scope': 'smtp',
    'name': 'host_url',
    'friendly_name': 'SMTP Server Name',
    'description': 'Host URL (default: empty)',
    'value': '',
    'is_protected': false
  },
  {
    'scope': 'smtp',
    'name': 'host_port',
    'friendly_name': 'SMTP Server Port',
    'description': 'Host PORT (default: 587)',
    'value': '587 ',
    'is_protected': false
  },
  {
    'scope': 'smtp',
    'name': 'host_ssl',
    'friendly_name': 'Secured Version',
    'description': 'If secured version of SMTP is used (default: false)',
    'value': 'false',
    'is_protected': false
  },
  {
    'scope': 'smtp',
    'name': 'sender_email',
    'friendly_name': 'SMTP Sender Email',
    'description': 'Sender Email Address (default: sentinel.notification@example.com)',
    'value': 'sentinel.notification@example.com',
    'is_protected': false
  },
  {
    'scope': 'smtp',
    'name': 'sender_name',
    'friendly_name': 'SMTP Sender Name',
    'description': 'Sender Name (default: Sentinel Notification)',
    'value': 'Sentinel Notification',
    'is_protected': false
  },
  {
    'scope': 'smtp',
    'name': 'auth',
    'friendly_name': 'SMTP Authentication',
    'description': 'If authentication to SMTP is needed (default: false)',
    'value': 'false',
    'is_protected': false
  },
  {
    'scope': 'smtp',
    'name': 'username',
    'friendly_name': 'SMTP Account Username',
    'description': 'User with access to SMTP server (default: sentinel@example.com)',
    'value': 'sentinel@example.com',
    'is_protected': false
  },
  {
    'scope': 'smtp',
    'name': 'password',
    'friendly_name': 'SMTP Account Password',
    'description': 'User password (default: password)',
    'value': 'password',
    'is_protected': false
  },
  { // PRINT
    'scope': 'print',
    'name': 'host_url',
    'friendly_name': 'Print Server Name',
    'description': 'Host URL (default: empty)',
    'value': '',
    'is_protected': false
  }
];

export default settingInitData;
