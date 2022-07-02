import SettingModel from '../setting.model.js';
import smtpData from './smtp.data.js';
import _ from 'underscore';

async function smtpScopeInit() {
  _.each(smtpData, async (data) => {
    var itemExist = await SettingModel.findOne({ scope: data.scope, name: data.name });
    if (!itemExist) { 
      var newItem = new SettingModel({ 
        scope: data.scope,
        name: data.name,
        friendly_name: data.friendly_name,
        description: data.description,
        value: data.value,
        protected: data.protected
      });
      await newItem.save();
    }
  });
};

export default smtpScopeInit;
