import _ from 'underscore';
import ActiveDirectory from 'activedirectory2';
// import ActiveDirectory from 'activedirectory2/lib/adpromise.js';
import { settingService, userService, departmentService } from './mongo.controller.js';

async function ldapParams() {
  var ldap_host_ssl = await settingService.getOne({ scope: 'ldap', name: 'host_ssl' });
  var ldap_host_url = await settingService.getOne({ scope: 'ldap', name: 'host_url' });
  var ldap_host_port = await settingService.getOne({ scope: 'ldap', name: 'host_port' });
  var ldap_username = await settingService.getOne({ scope: 'ldap', name: 'username' });
  var ldap_password = await settingService.getOne({ scope: 'ldap', name: 'password' });
  var ldap_basedn_root = await settingService.getOne({ scope: 'ldap', name: 'basedn_root' });
  var ldap_basedn_user = await settingService.getOne({ scope: 'ldap', name: 'basedn_users' });

  if (JSON.parse(ldap_host_ssl.value || false)) {
    var ldap_connection_string = `ldaps://${ldap_host_url.value}:${ldap_host_port.value}`;
  } else {
    var ldap_connection_string = `ldap://${ldap_host_url.value}:${ldap_host_port.value}`;
  }

  return {
    url: ldap_connection_string,
    username: ldap_username.value,
    password: ldap_password.value, // utils.decrypt(ldap_password.id, ldap_password.value),
    rootBaseDN: ldap_basedn_root.value,
    userBaseDN: ldap_basedn_user.value,
  }
}

async function rootBaseDn() {
  var params = await ldapParams();
  return new ActiveDirectory({
    url: params.url,
    baseDN: params.rootBaseDN,
    username: params.username,
    password: params.password
  });
}

async function userBaseDn() {
  var params = await ldapParams();
  return new ActiveDirectory({
    url: params.url,
    baseDN: params.userBaseDN,
    username: params.username,
    password: params.password
  });
}

async function userAttributes() {
  var attribute_user_cn = await settingService.getOne({ scope: 'ldap', name: 'attr_user_cn' });
  var attribute_user_dn = await settingService.getOne({ scope: 'ldap', name: 'attr_user_dn' });
  var attribute_user_userprincipalname = await settingService.getOne({ scope: 'ldap', name: 'attr_user_userprincipalname' });
  var attribute_user_samaccountname = await settingService.getOne({ scope: 'ldap', name: 'attr_user_samaccountname' });
  var attribute_user_mail = await settingService.getOne({ scope: 'ldap', name: 'attr_user_mail' });
  var attribute_user_mobile = await settingService.getOne({ scope: 'ldap', name: 'attr_user_mobile' });
  var attribute_user_telephonenumber = await settingService.getOne({ scope: 'ldap', name: 'attr_user_telephonenumber' });
  var attribute_user_objectsid = await settingService.getOne({ scope: 'ldap', name: 'attr_user_objectsid' });
  var attribute_user_departmentnumber = await settingService.getOne({ scope: 'ldap', name: 'attr_user_departmentnumber' });
  var attribute_user_manager = await settingService.getOne({ scope: 'ldap', name: 'attr_user_manager' });
  var attribute_user_givenname = await settingService.getOne({ scope: 'ldap', name: 'attr_user_givenname' });
  var attribute_user_sn = await settingService.getOne({ scope: 'ldap', name: 'attr_user_sn' });
  var attribute_user_useraccountcontrol = await settingService.getOne({ scope: 'ldap', name: 'attr_user_useraccountcontrol' });
  var attribute_user_employeenumber = await settingService.getOne({ scope: 'ldap', name: 'attr_user_employeenumber' });
  var attribute_user_title = await settingService.getOne({ scope: 'ldap', name: 'attr_user_title' });

  var attributes = {
    objectsid: attribute_user_objectsid.value,
    useraccountcontrol: attribute_user_useraccountcontrol.value,
    samaccountname: attribute_user_samaccountname.value,
    userprincipalname: attribute_user_userprincipalname.value,
    dn: attribute_user_dn.value,
    cn: attribute_user_cn.value,
    givenname: attribute_user_givenname.value,
    sn: attribute_user_sn.value,
    employeenumber: attribute_user_employeenumber.value,
    departmentnumber: attribute_user_departmentnumber.value,
    mail: attribute_user_mail.value,
    mobile: attribute_user_mobile.value,
    telephonenumber: attribute_user_telephonenumber.value,
    title: attribute_user_title.value,
    manager: attribute_user_manager.value
  };

  return attributes;
}

async function userOptions() {
  var attributes = await userAttributes();
  return {
    filter: 'objectClass=user',
    attributes: [
      attributes.objectsid,
      attributes.useraccountcontrol,
      attributes.samaccountname,
      attributes.userprincipalname,
      attributes.dn,
      attributes.cn,
      attributes.givenname,
      attributes.sn,
      attributes.employeenumber,
      attributes.departmentnumber,
      attributes.mail,
      attributes.mobile,
      attributes.telephonenumber,
      attributes.title,
      attributes.manager
    ], 
    paged:true
  };
}

function _isUserDisabled(accountcontrol) {
  
  // const DISABLED_MASK = 0x2;
  // let value = parseInt(accountcontrol);
  // let disabled = (value & DISABLED_MASK) === 0;
  // console.log((value & 0x0002) == 2);
  // console.log(!disabled);

  // console.log((accountcontrol & 0x0002) == 2);

  // return (accountcontrol & 0x0002);
  return ((accountcontrol & 0x0002) == 2);
}

// ? - maybe add feature - autocreate department
async function allUserSync() { // TODO - validate | after or before this run cleanup job - users which are in database as ldap_user and not fetched from AD move to deleted_department
  var use_sync = await settingService.getOne({ scope: 'ldap', name: 'use_sync' });
  if (JSON.parse(use_sync.value || false)) {
    console.log('Running - LDAP User Sync Cron Job');

    var ad = await userBaseDn();
    var opts = await userOptions();
    var attributes = await userAttributes();
    var unsortedDepartment = await departmentService.getOne({ department_number: '01_sentinel_unsorted_department' });
    var disabledDepartment = await departmentService.getOne({ department_number: '02_sentinel_disabled_department' });
    var deletedDepartment = await departmentService.getOne({ department_number: '03_sentinel_deleted_department' });
    var existingLdapUsers = await userService.get({ is_ldap_user: true }, { ldap_sid: 1 });
    
    _.each(existingLdapUsers, async (ldapUser) => {
      ad.userExists({ filter: `objectSid=${ldapUser.ldap_sid}` }, '*', async function(err, exists) {
        if (err) {
          console.log('USER EXIST - ERROR: ' + JSON.stringify(err)); // ?
          return;
        }
  
        if (!exists) { // user in database marked as ldap user is not found in active directory // TODO - handle sync to all apps / devices
          Object.assign(ldapUser, {
            department: deletedDepartment._id
          });
          await ldapUser.save();
        } else {
          // TODO - here maybe add AD container for deleted users - to check if user was moved to deleted container in AD to mark him as deleted in app
          // TODO - new database record for ldap needed
        }
      }); 
      // ad.findUsers({ filter: `objectSid=${ldapUser.ldap_sid}`, attributes: ['dn'] }, false, async function(err, users) {
      //   if (err) {
      //     console.log('ERROR: ' + JSON.stringify(err));
      //     return;
      //   }
  
      //   if ((!users) || (users.length == 0)) { // ? - here handle move to deleted_department for user with this ldap_sid
      //     Object.assign(ldapUser, {
      //       department: deletedDepartment._id
      //     });
      //     await ldapUser.save();
      //   }
      // });  
      // ad.findUser({ filter: `objectSid=${ldapUser.ldap_sid}`, attributes: ['dn'] }, '*', async function(err, users) {
      //   if (err) {
      //     console.log('ERROR: ' + JSON.stringify(err));
      //     return;
      //   }

      //   if ((!users) || (users.length == 0)) {
      //     Object.assign(ldapUser, {
      //       department: deletedDepartment._id
      //     });
      //     await ldapUser.save();
      //   } 
      // });  
    });

    ad.findUsers(opts, false, function(err, users) {
      if (err) {
        console.log('USER FIND - ERROR: ' + JSON.stringify(err)); // ?
        return;
      }
      if ((! users) || (users.length == 0)) {
        console.log('No LDAP users found.') // ?
      } else {
        _.each(users, async (user) => {
          if(user[attributes.employeenumber] == null || user[attributes.employeenumber] == undefined) { // user has no employeenumber // TODO - maybe add another filter based on user defined attribute (eg. only internal employee accounts)
            return; // skip this user
          } else {

            var departmentExist = await departmentService.exists({ ldap_sync: true, department_number: user[attributes.departmentnumber] });
            var userExist = await userService.exists({ ldap_sid: user[attributes.objectsid] });
            var userDisabled = _isUserDisabled(user[attributes.useraccountcontrol]);

            if (!userExist) { // ? - check if correct assign if departmentExist
              await userService.create({
                is_ldap_user: true,
                is_admin: false,
                login_enabled: false,
                is_protected: false,
                ldap_sid: user[attributes.objectsid],
                user_number: user[attributes.employeenumber],
                name: user[attributes.givenname],
                surname: user[attributes.sn],
                username: user[attributes.samaccountname], // username: user[attributes.userprincipalname],
                email: user[attributes.mail],
                mobile: user[attributes.mobile],
                telephone: user[attributes.telephonenumber],
                job_title: user[attributes.title],
                department: (userDisabled) ? disabledDepartment._id : (departmentExist ? departmentExist._id : unsortedDepartment._id)
                // manager // TODO
              });
            } else { // ? - check if correct assign if departmentExist and going from disabled to enabled or from deleted to reactivated
              Object.assign(userExist, { // TODO - change to service
                user_number: user[attributes.employeenumber],
                name: user[attributes.givenname],
                surname: user[attributes.sn],
                username: user[attributes.samaccountname], // username: user[attributes.userprincipalname],
                email: user[attributes.mail],
                mobile: user[attributes.mobile],
                telephone: user[attributes.telephonenumber],
                job_title: user[attributes.title],
                ...(userDisabled) && { department: disabledDepartment._id },
                ...(departmentExist && !userDisabled) && { department: departmentExist._id },
                ...(!departmentExist && !userDisabled && userExist.department.equals(disabledDepartment._id)) && { department: unsortedDepartment._id },
                ...(!departmentExist && !userDisabled && userExist.department.equals(deletedDepartment._id)) && { department: unsortedDepartment._id }
                // manager // TODO
              });
              await userExist.save();
            }
          }
        });
      }
    });
    console.log('Finished - LDAP User Sync Cron Job'); // TODO - socket emit for admin room
  } else {
    console.log('Disabled - LDAP User Sync Cron Job'); // TODO - socket emit for admin room
  }
}

async function oneUserSync() { // TODO
  return 'TODO';
}

function _authErrCodeParser(err) {
  if (err.lde_message) {
    return err.lde_message.split('data ')[1].split(',')[0];
  } else {
    if (err.code) {
      return err.code;
    } else {
      return 'unknown';
    }
  }
}

function authErrCode(err) {
  switch(_authErrCodeParser(err)) {
    case '52e': // ERROR_LOGON_FAILURE
      return 'Invalid password';
    case '52f': // ERROR_ACCOUNT_RESTRICTION
      return 'Account restrictions violated';
    case '531': // ERROR_INVALID_LOGON_HOURS
      return 'Logon time restriction violated';
    case '531': // ERROR_INVALID_WORKSTATION
      return 'Not allowed to log on to this computer'; // ? - probably STL-GW server
    case '532': // ERROR_PASSWORD_EXPIRED
      return 'Password expired';
    case '533': // ERROR_ACCOUNT_DISABLED
      return 'Account disabled';
    case '568': // ERROR_TOO_MANY_CONTEXT_IDS
      return 'Too many security identifiers accumulated';
    case '701': // ERROR_ACCOUNT_EXPIRED
      return 'Account expired';
    case '773': // ERROR_PASSWORD_MUST_CHANGE	
      return 'Password must be changed';
    case '775': // ERROR_ACCOUNT_LOCKED_OUT
      return 'Account locked';
    case 'ETIMEDOUT':
      return 'LDAP server unavailable';
    case 'ECONNREFUSED':
      return 'LDAP server refused connection';
    case 'unknown':
      return 'Unknown LDAP error';
    default:
      return 'Unhandled LDAP error';
  }
}

async function testConnection(callback) {
  var { username, password } = await ldapParams();
  var ad = await rootBaseDn();
  ad.authenticate(username, password, function(err, auth) {
    if (err) {
      return callback({ 
        status: 'NOK', 
        data: {
          message: `LDAP Connection Failed: ${authErrCode(err)}` 
        }
      });
    }
    if (auth) {
      callback({ 
        status: 'OK', 
        data: {
          message: 'LDAP Connection Successful!' 
        }
      });
    } else {
      callback({ 
        status: 'NOK', 
        data: {
          message: 'LDAP Authentication failed!' 
        }
      });
    } 
  });
}

export default {
  rootBaseDn,
  userBaseDn,
  ldapParams,
  userOptions,
  allUserSync,
  oneUserSync,
  authErrCode,
  testConnection,
  // groupOptions,
};
