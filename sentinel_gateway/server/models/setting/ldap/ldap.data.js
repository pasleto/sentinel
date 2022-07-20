const ldapData = [
  {
    'scope': 'ldap',
    'name': 'use_auth',
    'friendly_name': 'Use Authentication',
    'description': 'If LDAP authentication is enabled (default: false)',
    'value': 'false',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'use_sync',
    'friendly_name': 'Use Synchronization',
    'description': 'If LDAP synchronization is enabled (default: false)',
    'value': 'false',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'host_url',
    'friendly_name': 'LDAP Server Name',
    'description': 'Host URL (default: dc.example.com)',
    'value': 'dc.example.com',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'host_port',
    'friendly_name': 'LDAP Server Port',
    'description': 'Host PORT (default: 389)',
    'value': '389',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'host_ssl',
    'friendly_name': 'Secured Version',
    'description': 'If secured version of LDAP is used (default: false)',
    'value': 'false',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'username',
    'friendly_name': 'Administrator Account Username',
    'description': 'User with access to Active Directory (default: sentinel@example.com)',
    'value': 'sentinel@example.com',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'password',
    'friendly_name': 'Administrator Account Password',
    'description': 'User password (default: password)',
    'value': 'password',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'domain_dns_name',
    'friendly_name': 'Domain DNS Name',
    'description': 'Domain DNS Name (default: example.com)',
    'value': 'example.com',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'domain_netbios_name',
    'friendly_name': 'Domain NetBIOS Name',
    'description': 'Domain NetBIOS Name (default: example)',
    'value': 'example',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'basedn_root',
    'friendly_name': 'Root BaseDN',
    'description': 'Topmost container BaseDN (default: dc=example,dc=com)',
    'value': 'dc=example,dc=com',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'basedn_users',
    'friendly_name': 'Users BaseDN',
    'description': 'Users container BaseDN (default: ou=users,dc=example,dc=com)',
    'value': 'cn=users,dc=example,dc=com',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_cn',
    'friendly_name': 'User Attribute - cn',
    'description': 'User Attribute Mapping - cn (default: cn)',
    'value': 'cn',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_dn',
    'friendly_name': 'User Attribute - dn',
    'description': 'User Attribute Mapping - dn (default: dn)',
    'value': 'dn',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_userprincipalname',
    'friendly_name': 'User Attribute - userPrincipalName',
    'description': 'User Attribute Mapping - userPrincipalName (default: userPrincipalName)',
    'value': 'userPrincipalName',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_samaccountname',
    'friendly_name': 'User Attribute - sAMAccountName',
    'description': 'User Attribute Mapping - sAMAccountName (default: sAMAccountName)',
    'value': 'sAMAccountName',
    'protected': false
  },
  {
    'scope': 'ldap',
    'name': 'attr_user_description',
    'friendly_name': 'User Attribute - description',
    'description': 'User Attribute Mapping - description (default: description)',
    'value': 'description',
    'protected': false
  }

  // ,
  // {
  //   'scope': 'ldap',
  //   'name': 'attr_XXX',
  //   'friendly_name': 'XXX Attribute',
  //   'description': 'XXX Attribute Mapping (default: XXX)',
  //   'value': 'XXX',
  //   'protected': false
  // }
];

export default ldapData;
