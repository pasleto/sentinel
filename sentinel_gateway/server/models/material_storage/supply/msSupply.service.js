import MsSupplyModel from './msSupply.model.js';

async function exists(...params) {
  var result = await MsSupplyModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await MsSupplyModel.find(...params);
  if (!result) throw new Error('No supplies found in database!');
  return result;
};

async function getOne(...params) {
  var result = await MsSupplyModel.findOne(...params);
  if (!result) throw new Error('Supply not found in database!');
  return result;
};

async function create(params) {
  var newItem = new MsSupplyModel(params);
  await newItem.save();
};

export default {
  exists,
  get,
  getOne,
  create,
};