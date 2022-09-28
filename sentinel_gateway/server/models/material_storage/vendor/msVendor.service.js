import MsVendorModel from './msVendor.model.js';

async function exists(...params) {
  var result = await MsVendorModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await MsVendorModel.find(...params);
  if (!result) throw new Error('No vendors found in database!');
  return result;
};

async function getOne(...params) {
  var result = await MsVendorModel.findOne(...params);
  if (!result) throw new Error('Vendor not found in database!');
  return result;
};

async function create(params) {
  var newItem = new MsVendorModel(params);
  await newItem.save();
};

export default {
  exists,
  get,
  getOne,
  create,
};