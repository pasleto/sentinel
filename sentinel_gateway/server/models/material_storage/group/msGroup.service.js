import MsGroupModel from './msGroup.model.js';

async function exists(...params) {
  var result = await MsGroupModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await MsGroupModel.find(...params);
  if (!result) throw new Error('No groups found in database!');
  return result;
};

async function getOne(...params) {
  var result = await MsGroupModel.findOne(...params);
  if (!result) throw new Error('Group not found in database!');
  return result;
};

async function modify(id, params) { 
  var result = await MsGroupModel.findOne({_id: id});
  if (!result) throw new Error('Group not found in database!');
  Object.assign(result, params);
  await result.save();
};

async function create(params) {
  var newItem = new MsGroupModel(params);
  await newItem.save();
};

async function remove(id) {
  // TODO - delete group
  // check if any device is assigned
  // check if any supply is assigned
};

export default {
  exists,
  get,
  getOne,
  modify,
  create,
};