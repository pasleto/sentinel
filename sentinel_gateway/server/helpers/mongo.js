const mongoose = require('mongoose');
const logSymbols = require('log-symbols');

const User = require('../models/user.model');
const Setting = require('../models/setting.model');
const Department = require('../models/department.model');
const Employee = require('../models/employee.model');
const UnitType = require('../models/unitType.model');
const Unit = require('../models/unit.model');
const Card = require('../models/card.model');

async function connect() {
  mongoose.connect(process.env.MONGO_URI)
    .then((result) => {
      console.log(logSymbols.info, `Connected to MongoDB database [${result.connection.name}] at ${result.connection.host}:${result.connection.port}`);
      // mongoose.Schema.Types.String.checkRequired(v => v != null); 
      // mongoose.Schema.Types.String.checkRequired(v => typeof v === 'string'); // accept empty string
      _init();
    })
    .catch((err) => console.log(logSymbols.error, `Unable to connect to MongoDB database`, `\n${err}`));
};

async function _init() {
  // init database setup
  console.log('MONGO INIT');

  _settingInit();
  _unitTypesInit();

};

async function _settingInit() {
  const LDAP_URL = await Setting.findOne({ name: 'ldap.url' });
  if (!LDAP_URL) { 
    var ldapUrl = new Setting({ 
      name: 'ldap.url', 
      description: 'LDAP: Host URL (eg. ldap://dc.example.com)',
      value: ''
    });
    await ldapUrl.save();
  }

  const LDAP_BASE_DN = await Setting.findOne({ name: 'ldap.basedn' });
  if (!LDAP_BASE_DN) { 
    var ldapBaseDn = new Setting({ 
      name: 'ldap.basedn', 
      description: 'LDAP: BaseDN (eg. ou=users,dc=example,dc=com)',
      value: ''
    });
    await ldapBaseDn.save();
  }

  const LDAP_USERNAME = await Setting.findOne({ name: 'ldap.username' });
  if (!LDAP_USERNAME) { 
    var ldapUsername = new Setting({ 
      name: 'ldap.username', 
      description: 'LDAP: User with access to Active Directory (eg. ldapadmin@example.com)',
      value: ''
    });
    await ldapUsername.save();
  }

  const LDAP_PASSWORD = await Setting.findOne({ name: 'ldap.password' });
  if (!LDAP_PASSWORD) { 
    var ldapPassword = new Setting({ 
      name: 'ldap.password', 
      description: 'LDAP: User password',
      value: ''
    });
    await ldapPassword.save();
  }

  const LDAP_USER_ATTRIBUTES = await Setting.findOne({ name: 'ldap.user.attributes' }); // ? hide this || make each entry possible to change to custom value
  if (!LDAP_USER_ATTRIBUTES) { 
    var ldapUserAttributes = new Setting({ 
      name: 'ldap.user.attributes', 
      description: 'LDAP: List of user attributes to return (Not recommended to modify!)',
      value: 'dn,userPrincipalName,sAMAccountName,cn,employeeID,sn,givenName,mobile,mail,displayName,memberOf,telephoneNumber,lockoutTime'
    });
    await ldapUserAttributes.save();
  }

  // TODO

};

async function _unitTypesInit() {
  const ACCESS_UNIT = await UnitType.findOne({ name: 'Access' });
  if (!ACCESS_UNIT) { 
    var accessUnit = new UnitType({ 
      name: 'Access',
      description: 'Root Access Unit Type',
      is_root: true,
      is_protected: true,
      image: 'TODO' // TODO
    });
    await accessUnit.save();
  }

  const ATTENDANCE_UNIT = await UnitType.findOne({ name: 'Attendance' });
  if (!ATTENDANCE_UNIT) { 
    var attendanceUnit = new UnitType({ 
      name: 'Attendance',
      description: 'Root Attendance Unit Type',
      is_root: true,
      is_protected: true,
      image: 'TODO' // TODO
    });
    await attendanceUnit.save();
  }
};

async function _userInit() {
  // const MASTER_USER = await User.findOne({ username: 'master' });
  // if (!MASTER_USER) { 
  //   var masterUser = new User({ 
  //     username: 'master', 
  //     password: '',
  //   });
  //   await masterUser.save();
  // }

  // TODO

};

module.exports = {
  connect,
  User,
  Setting,
  Department,
  Employee,
  UnitType,
  Unit,
  Card,
};