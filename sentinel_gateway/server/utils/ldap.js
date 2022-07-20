import _ from 'underscore';
import ActiveDirectory from 'activedirectory2';
import { SettingModel } from './mongo.js';

async function rootBaseDn() {
  var ldap_host_ssl = await SettingModel.findOne({ scope: 'ldap', name: 'host_ssl' });
  var ldap_host_url = await SettingModel.findOne({ scope: 'ldap', name: 'host_url' });
  var ldap_host_port = await SettingModel.findOne({ scope: 'ldap', name: 'host_port' });
  var ldap_username = await SettingModel.findOne({ scope: 'ldap', name: 'username' });
  var ldap_password = await SettingModel.findOne({ scope: 'ldap', name: 'password' });
  var ldap_basedn_root = await SettingModel.findOne({ scope: 'ldap', name: 'basedn_root' });

  if (ldap_host_ssl.value === 'true') { // if (JSON.parse(ldap_host_ssl.value)) {
    var ldap_connection_string = `ldaps://${ldap_host_url.value}:${ldap_host_port.value}`;
  } else {
    var ldap_connection_string = `ldap://${ldap_host_url.value}:${ldap_host_port.value}`;
  }

  // var port_number = parseInt(ldap_host_port); // ?

  // var config = {
  //   url: 'ldaps://ad.pasler.org:636',
  //   baseDN: 'dc=ad,dc=pasler,dc=org',
  //   username: 'MasterAdmin@ad.pasler.org',
  //   password: 'Tomas20398',
  // };

  var config = {
    url: ldap_connection_string,
    baseDN: ldap_basedn_root.value,
    username: ldap_username.value,
    password: ldap_password.value,
  };

  return new ActiveDirectory(config);
}

async function userBaseDn() {
  var ldap_host_ssl = await SettingModel.findOne({ scope: 'ldap', name: 'host_ssl' });
  var ldap_host_url = await SettingModel.findOne({ scope: 'ldap', name: 'host_url' });
  var ldap_host_port = await SettingModel.findOne({ scope: 'ldap', name: 'host_port' });
  var ldap_username = await SettingModel.findOne({ scope: 'ldap', name: 'username' });
  var ldap_password = await SettingModel.findOne({ scope: 'ldap', name: 'password' });
  var ldap_basedn_user = await SettingModel.findOne({ scope: 'ldap', name: 'basedn_users' });

  if (ldap_host_ssl.value === 'true') { // if (JSON.parse(ldap_host_ssl.value)) {
    var ldap_connection_string = `ldaps://${ldap_host_url.value}:${ldap_host_port.value}`;
  } else {
    var ldap_connection_string = `ldap://${ldap_host_url.value}:${ldap_host_port.value}`;
  }

  // var config = {
  //   url: 'ldaps://ad.pasler.org:636',
  //   baseDN: 'cn=users,dc=ad,dc=pasler,dc=org',
  //   username: 'MasterAdmin@ad.pasler.org',
  //   password: 'Tomas20398',
  // };  

  var config = {
    url: ldap_connection_string,
    baseDN: ldap_basedn_user.value,
    username: ldap_username.value,
    password: ldap_password.value,
  };

  return new ActiveDirectory(config);
}

async function userOptions() {
  var attribute_user_cn = await SettingModel.findOne({ scope: 'ldap', name: 'attr_user_cn' });
  var attribute_user_dn = await SettingModel.findOne({ scope: 'ldap', name: 'attr_user_dn' });
  var attribute_user_userprincipalname = await SettingModel.findOne({ scope: 'ldap', name: 'attr_user_userprincipalname' });
  var attribute_user_samaccountname = await SettingModel.findOne({ scope: 'ldap', name: 'attr_user_samaccountname' });
  var attribute_user_description = await SettingModel.findOne({ scope: 'ldap', name: 'attr_user_description' });
  // TODO


  var user_attributes = [
    attribute_user_cn.value,
    attribute_user_dn.value,
    attribute_user_userprincipalname.value,
    attribute_user_samaccountname.value,
    attribute_user_description.value
  ];

  return {
    filter: 'objectClass=user',
    attributes: user_attributes, 
    paged:true
  };
}

async function groupOptions() {

  var group_attributes = ['cn', 'description', 'distinguishedName', 'dn', 'objectCategory'];

  // TODO
  return {
    filter: 'objectClass=group',
    attributes: group_attributes,
    paged:true
  };
}

export default {
  rootBaseDn,
  userBaseDn,
  userOptions,
  groupOptions,
};
