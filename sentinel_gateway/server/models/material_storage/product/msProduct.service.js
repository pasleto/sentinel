import MsProductModel from './msProduct.model.js';

async function exists(...params) {
  var result = await MsProductModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await MsProductModel.find(...params);
  if (!result) throw new Error('No products found in database!');
  return result;
};

async function getOne(...params) {
  var result = await MsProductModel.findOne(...params);
  if (!result) throw new Error('Product not found in database!');
  return result;
};

async function create(params) {
  var newItem = new MsProductModel(params);
  await newItem.save();
};

export default {
  exists,
  get,
  getOne,
  create,
};
