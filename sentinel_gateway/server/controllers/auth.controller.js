import _ from 'underscore';
import ldap from './ldap.controller.js';
import utils from '../utils/util.js';
import { userService, settingService, cardService, msDeviceService } from './mongo.controller.js';

// TODO - implement last_login_timestamp

// async function clientAppPasswordAuthenticate(user_username, user_password , callback) {
//   try {
//     var user = await userService.getOne({ username: user_username});
//     if (user.login_enabled) {
//       if (user.is_ldap_user) {
//         var ldap_auth = await settingService.getOne({scope: 'ldap', name: 'use_auth'});
//         if (JSON.parse(ldap_auth.value || false)) {
//           var ad = await ldap.userBaseDn();
//           ad.findUser({ filter: `objectSid=${user.ldap_sid}`, attributes: ['userPrincipalName'] }, '*', async function(err, ldap_user) {  // find user based on objectSid and authenticate using real username - if ever username change in AD and not synced to DB yet -< login using old not synced username yet
//             if (err) {
//               return callback({ 
//                 status: 'NOK', 
//                 data: {
//                   message: ldap.authErrCode(err) 
//                 }
//               });
//             }
//             if (!ldap_user) {
//               return callback({ 
//                 status: 'NOK', 
//                 data: {
//                   message: 'User found in database but not in LDAP!' 
//                 }
//               });
//             } else {
//               ad.authenticate(ldap_user.userPrincipalName, user_password, function(err, auth) {
//                 if (err) {
//                   return callback({ 
//                     status: 'NOK', 
//                     data: {
//                       message: ldap.authErrCode(err) 
//                     }
//                   });
//                 }
//                 if (auth) {
//                   callback({ 
//                     status: 'OK', 
//                     data: {
//                       user: _.omit(user.toObject(), 'password', 'createdAt', 'updatedAt', '__v')  // ? - check what else omit
//                     }
//                   });
//                 } else {
//                   callback({ 
//                     status: 'NOK', 
//                     data: {
//                       message: 'LDAP Authentication failed!' 
//                     }
//                   });
//                 }
//               });
//             }
//           });
//         } else {
//           throw new Error('Authentication using LDAP is globally disabled!');
//         }
//       } else {
//         if (utils.comparePasswordHash(user_password, user.password)) {
//           callback({ 
//             status: 'OK', 
//             data: {
//               user: _.omit(user.toObject(), 'password', 'createdAt', 'updatedAt', '__v')  // ? - check what else omit
//             }
//           });
//         } else {
//           throw new Error('Wrong password!');
//         }
//       }
//     } else {
//       throw new Error('Your are not allowed to login to this application!');
//     }
//   } catch (error) {
//     callback({ 
//       status: 'NOK', 
//       data: {
//         message: error.message 
//       }
//     });
//   }
// }

// async function clientAppPasswordlessAuthenticate(user_sid, callback) { // ? - unsecure - anyone can pass object_sid and will grand access
//   try {
//     var user = await userService.getOne({ ldap_sid: user_sid });
//     if (user.is_ldap_user) {
//       if (user.login_enabled) {
//         var ldap_auth = await settingService.getOne({scope: 'ldap', name: 'use_auth'});
//         if (JSON.parse(ldap_auth.value || false)) {
//           var ad = await ldap.userBaseDn();
//           ad.findUser({ filter: `objectSid=${user.ldap_sid}`, attributes: ['userPrincipalName'] }, '*', async function(err, ldap_user) {
//             if (err) {
//               return callback({ 
//                 status: 'NOK', 
//                 data: {
//                   message: ldap.authErrCode(err) 
//                 }
//               });
//             }
//             if (!ldap_user) {
//               return callback({ 
//                 status: 'NOK', 
//                 data: {
//                   message: 'User found in database but not in LDAP!' 
//                 }
//               });
//             } else {
//               callback({ 
//                 status: 'OK', 
//                 data: {
//                   user: _.omit(user.toObject(), 'password', 'createdAt', 'updatedAt', '__v')  // ? - check what else omit
//                 }
//               });
//             }
//           });
//         } else {
//           throw new Error('Authentication using LDAP is globally disabled!');
//         }
//       } else {
//         throw new Error('Your are not allowed to login to this application!');
//       }
//     } else {
//       throw new Error('User found, but not marked as LDAP user!');
//     }
//   } catch (error) {
//     callback({ 
//       status: 'NOK', 
//       data: {
//         message: error.message 
//       }
//     });
//   }
// }

async function materialStoragePasswordAuthenticate(user_username, user_password, deviceID, callback) { // TODO
  try {
    var device = await msDeviceService.getOne({device_id: deviceID});
    if (device.groups && device.groups.length > 0) {
      var user = await userService.getOne(false, false, { username: user_username });
      await device.populate({
        path: 'groups',
        select: '-devices -supplies -createdAt -updatedAt -__v'
      });

      // TODO - check permissions to groups assigned to device
      // TODO - return array of groups which user has permissions to access
      // TODO - if no permissions return error
      // TODO - check for for admin aswell

      if (user.is_ldap_user) {
        var ldap_auth = await settingService.getOne({scope: 'ldap', name: 'use_auth'});
        if (JSON.parse(ldap_auth.value || false)) {
          var ad = await ldap.userBaseDn();
          ad.findUser({ filter: `objectSid=${user.ldap_sid}`, attributes: ['userPrincipalName'] }, '*', async function(err, ldap_user) {  // find user based on objectSid and authenticate using real username - if ever username change in AD and not synced to DB yet -< login using old not synced username yet
            if (err) {
              return callback({ 
                status: 'NOK', 
                data: {
                  message: ldap.authErrCode(err) 
                }
              });
            }
            if (!ldap_user) {
              return callback({ 
                status: 'NOK', 
                data: {
                  message: 'User found in database but not in LDAP!' 
                }
              });
            } else {
              ad.authenticate(ldap_user.userPrincipalName, user_password, function(err, auth) {
                if (err) {
                  return callback({ 
                    status: 'NOK', 
                    data: {
                      message: ldap.authErrCode(err) 
                    }
                  });
                }
                if (auth) {
                  callback({ 
                    status: 'OK', 
                    data: {
                      user: _.omit(user.toObject(), 'password', 'createdAt', 'updatedAt', '__v'),
                      groups: device.groups.toObject()
                    }
                  }); // ? - check what else omit
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
          });
        } else {
          throw new Error('Authentication using LDAP is globally disabled!');
        }
      } else {
        if (user.password) {
          if (utils.comparePasswordHash(user_password, user.password)) {
            callback({ 
              status: 'OK', 
              data: {
                user: _.omit(user.toObject(), 'password', 'createdAt', 'updatedAt', '__v'),
                groups: device.groups.toObject()
              }
            }); // ? - check what else omit
          } else {
            throw new Error('Wrong password!');
          }
        } else {
          throw new Error('You have no password set!');
        }
      }
    } else {
      throw new Error('No groups assigned to this device!');
    }
  } catch (error) {
    callback({ 
      status: 'NOK', 
      data: {
        message: error.message 
      }
    });
  }
}

async function materialStorageCardAuthenticate(card, deviceID, callback) { // TODO
  try {
    var device = await msDeviceService.getOne({device_id: deviceID});
    if (device.groups && device.groups.length > 0) {
      var userCard = await cardService.getOne({value: card});
      await cardService.checkValidity(userCard._id); // TODO
      var user = await userService.getOne(false, false, { _id: userCard.user} );
      await device.populate({
        path: 'groups',
        select: '-devices -supplies -createdAt -updatedAt -__v'
      });

      // TODO - check permissions to groups assigned to device
      // TODO - return array of groups which user has permissions to access
      // TODO - if no permissions return error
      // TODO - check for for admin aswell
      
      callback({ 
        status: 'OK', 
        data: {
          user: _.omit(user.toObject(), 'password', 'createdAt', 'updatedAt', '__v'),
          groups: device.groups.toObject()
        }
      }); // ? - check what else omit
    } else {
      throw new Error('No groups assigned to this device!');
    }
  } catch (error) {
    callback({ 
      status: 'NOK', 
      data: {
        message: error.message 
      }
    });
  }
}

async function materialStorageCardBarcodeAuthenticate(cryptedCard, deviceID, callback) { // TODO
  try {
    var device = await msDeviceService.getOne({device_id: deviceID});
    if (device.groups && device.groups.length > 0) {
      var decryptedCard = await utils.decryptCard(cryptedCard);
      var userCard = await cardService.getOne({value: decryptedCard});
      await cardService.checkValidity(userCard._id); // TODO
      var user = await userService.getOne(false, false, { _id: userCard.user });
      await device.populate({
        path: 'groups',
        select: '-devices -supplies -createdAt -updatedAt -__v'
      });

      // TODO - check permissions to groups assigned to device
      // TODO - return array of groups which user has permissions to access
      // TODO - if no permissions return error
      // TODO - check for for admin aswell

      callback({ 
        status: 'OK', 
        data: {
          user: _.omit(user.toObject(), 'password', 'createdAt', 'updatedAt', '__v'),
          groups: device.groups.toObject()
        }
      }); // ? - check what else omit
    } else {
      throw new Error('No groups assigned to this device!');
    }
  } catch (error) {
    callback({ 
      status: 'NOK', 
      data: {
        message: error.message 
      }
    });
  }
}

export default {
  // clientAppPasswordAuthenticate,
  // clientAppPasswordlessAuthenticate,
  materialStoragePasswordAuthenticate,
  materialStorageCardAuthenticate,
  materialStorageCardBarcodeAuthenticate,
};