import MsDeviceModel from './msDevice.model.js';

async function exists(...params) {
  var result = await MsDeviceModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await MsDeviceModel.find(...params);
  if (!result) throw new Error('No devices found in database!');
  return result;
};

async function getOne(...params) {
  var result = await MsDeviceModel.findOne(...params);
  if (!result) throw new Error('Device not found in database!');
  return result;
};

async function modify(id, params) {
  var result = await MsDeviceModel.findOne({_id: id});
  if (!result) throw new Error('Device not found in database!');
  Object.assign(result, params);
  await result.save();
};

async function create(params) {
  var newItem = new MsDeviceModel(params);
  await newItem.save();
};

async function remove(id) {
  var result = await MsDeviceModel.findOne({_id: id});
  if (!result) throw new Error('Device not found in database!');
  await result.remove();
};

export default {
  exists,
  get,
  getOne,
  modify,
  create,
  remove,
};