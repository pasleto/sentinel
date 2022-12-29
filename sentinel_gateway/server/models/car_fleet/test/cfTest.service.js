import CfTestModel from './cfTest.model.js';

async function exists(...params) {
  var result = await CfTestModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await CfTestModel.find(...params);
  if (!result) throw new Error('No CfTest found in database!');
  return result;
};

async function getOne(...params) {
  var result = await CfTestModel.findOne(...params);
  if (!result) throw new Error('CfTest not found in database!');
  return result;
};

async function create(params) {
  var newItem = new CfTestModel(params);
  await newItem.save();
};

export default {
  exists,
  get,
  getOne,
  create,
};