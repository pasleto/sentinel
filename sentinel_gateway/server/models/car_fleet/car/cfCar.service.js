import CfCarModel from './cfCar.model.js';

async function exists(...params) {
  var result = await CfCarModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(lean, populateParams, ...findParams) { // getLean
  var result = await CfCarModel.find(...findParams).populate(populateParams).lean(lean);
  if (!result) throw new Error('No cars found in database!');
  return result;
};

// async function get(...params) {
//   var result = await CfCarModel.find(...params);
//   if (!result) throw new Error('No cars found in database!');
//   return result;
// };

async function getOne(lean, populateParams, findParams) {
  var result = await CfCarModel.findOne(findParams).populate(populateParams).lean(lean);
  if (!result) throw new Error('Car not found in database!');
  return result;
};

// async function getOne(...params) {
//   var result = await CfCarModel.findOne(...params);
//   if (!result) throw new Error('Car not found in database!');
//   return result;
// };

async function modify(id, params) { 
  var result = await CfCarModel.findOne({_id: id});
  if (!result) throw new Error('Car not found in database!');
  Object.assign(result, params);
  await result.save();
};

async function create(params) {
  var newItem = new CfCarModel(params);
  await newItem.save();
};

async function remove(id) {
  var result = await CfCarModel.findOne({_id: id});
  if (!result) throw new Error('Car not found in database!');
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