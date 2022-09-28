import MsSupplierModel from './msSupplier.model.js';

async function exists(...params) {
  var result = await MsSupplierModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await MsSupplierModel.find(...params);
  if (!result) throw new Error('No suppliers found in database!');
  return result;
};

async function getOne(...params) {
  var result = await MsSupplierModel.findOne(...params);
  if (!result) throw new Error('Supplier not found in database!');
  return result;
};

async function create(params) {
  var newItem = new MsSupplierModel(params);
  await newItem.save();
};

export default {
  exists,
  get,
  getOne,
  create,
};
