import mongoose from 'mongoose';
import logSymbols from 'log-symbols';

import TimezoneModel from '../models/timezone/timezone.model.js';
import timezoneModelInit from '../models/timezone/timezone.init.js';
import SettingModel from '../models/setting/setting.model.js';
import settingModelInit from '../models/setting/setting.init.js';
import AccessControlInitiatorModel from '../models/access_control/initiator/accessControlInitiator.model.js';
import accessControlInitiatorModelInit from '../models/access_control/initiator/accessControlInitiator.init.js';
import AccessControlEventModel from '../models/access_control/event/accessControlEvent.model.js';
import accessControlEventModelInit from '../models/access_control/event/accessControlEvent.init.js';


import util from './util.js';
import ldap from './ldap.js';

async function mongoConnect() {
  mongoose.connect(process.env.MONGO_URI || 'mongodb://localhost:27017/sentinel')
    .then((result) => {
      console.log(logSymbols.info, `Connected to MongoDB: ${result.connection.host}:${result.connection.port}/${result.connection.name}`);
      _init();
    })
    .catch((err) => console.log(logSymbols.error, `Unable to connect to MongoDB database`, `\n${err}`));
};

async function _init() { // init database structure
  timezoneModelInit();
  settingModelInit();
  accessControlInitiatorModelInit();
  accessControlEventModelInit();

  // TODO



  // ? - Testing 
  
  // var ad = await ldap.rootBaseDn();

  // var ad = await ldap.userBaseDn();

  // var opts = await ldap.userOptions();

  // ad.findUsers(opts, true, function(err, users) {
  //   if (err) {
  //     console.log('ERROR: ' +JSON.stringify(err));
  //     return;
  //   }
  
  //   if ((! users) || (users.length == 0)) console.log('No users found.');
  //   else {
  //     console.log(users.length);
  //   }
  // });

  console.log(util.getServerMacAddress());
  console.log(await util.getServerHostname());
  console.log(await util.getServerDomain());
  console.log(await util.getServerDomainHostname());
  console.log(await util.getServerUpdatesCount());

  console.log(await ldap.userOptions());

};

export {
  mongoConnect,
  TimezoneModel,
  SettingModel,
  AccessControlInitiatorModel,
  AccessControlEventModel,
};
