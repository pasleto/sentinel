import CfDeviceModel from './cfDevice.model.js';

async function exists(...params) {
  var result = await CfDeviceModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await CfDeviceModel.find(...params);
  if (!result) throw new Error('No CfDevice found in database!');
  return result;
};

async function getOne(...params) {
  var result = await CfDeviceModel.findOne(...params);
  if (!result) throw new Error('CfDevice not found in database!');
  return result;
};

async function create(params) {
  var newItem = new CfDeviceModel(params);
  await newItem.save();
};

export default {
  exists,
  get,
  getOne,
  create,
};