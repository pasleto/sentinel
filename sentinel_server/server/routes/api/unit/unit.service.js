const error = require('../../../helpers/error');
const mongo = require('../../../helpers/mongo');
const UnitType = mongo.UnitType;
const Unit = mongo.Unit;

async function getAll() {
  return await Unit.find();
}

async function getOneById(id) {
  const result = await Unit.findById(id);
  if (!result) throw new error.NotFoundError('Unit');
  return result;
}

async function updateOneById(id, body) {
  const result = await Unit.findById(id);
  if (!result) throw new error.NotFoundError('Unit');
  Object.assign(result, body);
  return await result.save();
}

async function deleteOneById(id) {
  return await Unit.findByIdAndDelete(id);
}

async function deleteAll() {
  // TODO
}

async function create(body) {
  const result = new Unit(body);
  return await result.save();
}

async function getAllTypes() {
  return await UnitType.find();
}

async function getAllTypesFull() {
  return await UnitType.find().populate('parent');
}

async function getOneTypeById(id) {
  const result = await UnitType.findById(id);
  if (!result) throw new error.NotFoundError('UnitType');
  return result;
}

async function getOneTypeByIdFull(id) {
  const result = await UnitType.findById(id).populate('parent');
  if (!result) throw new error.NotFoundError('UnitType');
  return result;
}

async function updateOneTypeById(id, body) {
  const result = await UnitType.findById(id);
  if (!result) throw new error.NotFoundError('UnitType');
  Object.assign(result, body);
  return await result.save();
}

async function deleteOneTypeById(id) { // TODO - check if any unit is of this type & handle this !
  return await UnitType.deleteOne({ is_protected: false, _id: id });
}

async function deleteAllTypes() { // TODO - check if any unit is of this type & handle this !
  return await UnitType.deleteMany({ is_protected: false });
}

async function createType(body) {
  const result = new UnitType(body);
  return await result.save();
}

module.exports = {
  getAll,
  getOneById,
  updateOneById,
  deleteOneById,  
  deleteAll,
  create,
  getAllTypes,
  getAllTypesFull,
  getOneTypeById,
  getOneTypeByIdFull,
  createType,
  updateOneTypeById,
  deleteAllTypes,
  deleteOneTypeById,
};