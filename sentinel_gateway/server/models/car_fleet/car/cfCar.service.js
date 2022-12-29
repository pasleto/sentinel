import CfCarModel from './cfCar.model.js';

async function exists(...params) {
  var result = await CfCarModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await CfCarModel.find(...params);
  if (!result) throw new Error('No CfCar found in database!');
  return result;
};

async function getOne(...params) {
  var result = await CfCarModel.findOne(...params);
  if (!result) throw new Error('CfCar not found in database!');
  return result;
};

async function create(params) {
  var newItem = new CfCarModel(params);
  await newItem.save();
};

export default {
  exists,
  get,
  getOne,
  create,
};