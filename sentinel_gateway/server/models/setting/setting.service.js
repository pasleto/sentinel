import SettingModel from './setting.model.js';
import settingInitData from './setting.init.js';
import util from '../../utils/util.js'
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
  _initRootCertificate();
};

async function _initRootCertificate() {
  var root_cert_exist = await SettingModel.exists({ scope: 'general', name: 'root_certificate' });
  var root_key_exist = await SettingModel.exists({ scope: 'general', name: 'root_private_key' });

  if (!root_cert_exist && !root_key_exist) {
    var root_cert_pem = util.generateRootCertificate('Sentinel Root CA');

    var root_cert = new SettingModel({
      scope: 'general',
      name: 'root_certificate',
      friendly_name: 'Root Certificate',
      description: 'Root Certificate',
      value: root_cert_pem.certificate,
      is_protected: true // ?
    });
    await root_cert.save();

    var root_key = new SettingModel({
      scope: 'general',
      name: 'root_private_key',
      friendly_name: 'Root Private Key',
      description: 'Root Private Key',
      value: root_cert_pem.privateKey,
      is_protected: true // ?
    });
    await root_key.save();
  }
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
