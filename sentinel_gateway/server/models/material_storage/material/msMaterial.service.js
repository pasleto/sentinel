import MsMaterialModel from './msMaterial.model.js';

async function exists(...params) {
  var result = await MsMaterialModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await MsMaterialModel.find(...params);
  if (!result) throw new Error('No materials found in database!');
  return result;
};

async function getOne(...params) {
  var result = await MsMaterialModel.findOne(...params);
  if (!result) throw new Error('Material not found in database!');
  return result;
};

async function create(params) {
  var newItem = new MsMaterialModel(params);
  await newItem.save();
};

export default {
  exists,
  get,
  getOne,
  create,
};
