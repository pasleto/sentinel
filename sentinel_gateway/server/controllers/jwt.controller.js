import jwt from 'jsonwebtoken';
import ldap from './ldap.controller.js';
import utils from '../utils/util.js';
import { userService, settingService, cardService, msDeviceService  } from './mongo.controller.js';

async function clientAppPasswordAuthenticate(user_username, user_password , callback) {
  try {
    var user = await userService.getOne(false, false, { username: user_username });
    var hash_jwt = await settingService.getOne({ scope: 'general', name: 'hash_jwt' });
    var expiration_jwt = await settingService.getOne({ scope: 'general', name: 'expiration_jwt' });
    if (user.login_enabled) {
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
                      token: jwt.sign({ user_id: user.id}, hash_jwt.value, { expiresIn: expiration_jwt.value }),
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
          });
        } else {
          throw new Error('Authentication using LDAP is globally disabled!');
        }
      } else {
        if (utils.comparePasswordHash(user_password, user.password)) {
          callback({ 
            status: 'OK', 
            data: {
              token: jwt.sign({ user_id: user.id}, hash_jwt.value, { expiresIn: expiration_jwt.value }),
            }
          });
        } else {
          throw new Error('Wrong password!');
        }
      }
    } else {
      throw new Error('Your are not allowed to login to this application!');
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
  clientAppPasswordAuthenticate,
};