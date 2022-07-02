import mongoose from 'mongoose';
import logSymbols from 'log-symbols';

import TimezoneModel from '../models/timezone/timezone.model.js';
import timezoneModelInit from '../models/timezone/timezone.init.js';
import SettingModel from '../models/setting/setting.model.js';
import settingModelInit from '../models/setting/setting.init.js';
import AccessControlInitiatorModel from '../models/accessControl/initiator/accessControlInitiator.model.js';
import accessControlInitiatorInit from '../models/accessControl/initiator/accessControlInitiator.init.js';
import AccessControlEventModel from '../models/accessControl/event/accessControlEvent.model.js';
import accessControlEventInit from '../models/accessControl/event/accessControlEvent.init.js';

async function mongoConnect() {
  mongoose.connect(process.env.MONGO_URI)
    .then((result) => {
      console.log(logSymbols.info, `Connected to MongoDB database [${result.connection.name}] at ${result.connection.host}:${result.connection.port}`);
      _init();
    })
    .catch((err) => console.log(logSymbols.error, `Unable to connect to MongoDB database`, `\n${err}`));
};

async function _init() { // init database structure
  timezoneModelInit();
  settingModelInit();
  accessControlInitiatorInit();
  accessControlEventInit();

  // TODO

};

export {
  mongoConnect,
  TimezoneModel,
  SettingModel,
  AccessControlInitiatorModel,
  AccessControlEventModel,
};
