const error = require('../../../helpers/error');
const mongo = require('../../../helpers/mongo');
const Setting = mongo.Setting;

/*
  Settings are setup on installation or update - only value change/clear is allowed
*/

async function getAll() {
  return await Setting.find();
}

async function getOneById(id) {
  const result = await Setting.findById(id);
  if (!result) throw new error.NotFoundError('Setting');
  return result;
}

async function updateOneById(id, body) {
  const result = await Setting.findById(id);
  if (!result) throw new error.NotFoundError('Setting');
  result.value = body.value; // allow only value to be changed
  return await result.save();
}

async function deleteOneById(id) {
  const result = await Setting.findById(id);
  if (!result) throw new error.NotFoundError('Setting');
  result.value = ''; // clear value
  return await result.save();
}

module.exports = {
  getAll,
  getOneById,
  updateOneById,
  deleteOneById,
};