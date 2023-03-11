import CfDeviceModel from './cfDevice.model.js';

async function exists(...params) {
  var result = await CfDeviceModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(lean, populateParams, ...findParams) { // getLean
  var result = await CfDeviceModel.find(...findParams).populate(populateParams).lean(lean);
  if (!result) throw new Error('No devices found in database!');
  return result;
};

// async function get(...params) {
//   var result = await CfDeviceModel.find(...params);
//   if (!result) throw new Error('No devices found in database!');
//   return result;
// };

async function getOne(lean, populateParams, ...findParams) { // TODO ...findParams to all tables
  var result = await CfDeviceModel.findOne(...findParams).populate(populateParams).lean(lean);
  if (!result) throw new Error('Device not found in database!');
  return result;
};

// async function getOne(...params) {
//   var result = await CfDeviceModel.findOne(...params);
//   if (!result) throw new Error('Device not found in database!');
//   return result;
// };

async function modify(id, params) { 
  var result = await CfDeviceModel.findOne({_id: id});
  if (!result) throw new Error('Device not found in database!');
  Object.assign(result, params);
  await result.save();
};

async function create(params) {
  var newItem = new CfDeviceModel(params);
  await newItem.save();
};

async function remove(id) {
  var result = await CfDeviceModel.findOne({_id: id});
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