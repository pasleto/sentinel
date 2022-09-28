import SettingModel from './setting.model.js';
import settingInitData from './setting.init.js';
import _ from 'underscore';

async function init() {
  _.each(settingInitData, async (data) => {
    var itemExist = await SettingModel.exists({ scope: data.scope, name: data.name });
    if (!itemExist) { 
      var newItem = new SettingModel({ 
        scope: data.scope,
        name: data.name,
        friendly_name: data.friendly_name,
        description: data.description,
        value: data.value,
        is_protected: data.is_protected
      });
      await newItem.save();
    }
  });
};

async function get(params) {
  var result = await SettingModel.find(params);
  if (!result) throw new Error('No settings found in database!');
  return result;
};

async function getOne(params) {
  var result = await SettingModel.findOne(params);
  if (!result) throw new Error('Setting record not found in database!');
  return result;
};

export default {
  init,
  get,
  getOne,
};
