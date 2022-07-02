const error = require('../../../helpers/error');
const mongo = require('../../../helpers/mongo');
const Card = mongo.Card;

async function getAll() {
  return await Card.find();
}

async function getAllFull() {
  return await Card.find().populate('employee');
}

async function getAllUnassigned() {
  return await Card.find({ employee: null });
}

async function getAllActive() {
  return await Card.find({ is_active: true }).populate('employee');
}

async function getAllUnactive() {
  return await Card.find({ is_active: false }).populate('employee');
}

async function getOneById(id) {
  const result = await Card.findById(id);
  if (!result) throw new error.NotFoundError('Card');
  return result;
}

async function getOneByIdFull(id) {
  const result = await Card.findById(id).populate('employee');
  if (!result) throw new error.NotFoundError('Card');
  return result;
}

async function getOneByHexUid(uid) {
  const result = await Card.find({ uid_hex: uid }).populate('employee');
  if (!result || result.length === 0) throw new error.NotFoundError('Card');
  return result;
}

async function updateOneById(id, body) {
  const result = await Card.findById(id);
  if (!result) throw new error.NotFoundError('Card');
  Object.assign(result, body);
  return await result.save();
}

async function returnOneById(id) { // TODO - set active to false, set is_returned to true, set primary false if set && remove from all units
  const result = await Card.findById(id).populate('employee');
  if (!result) throw new error.NotFoundError('Card');
  if (result.employee != null) throw new error.ConflictError('Unable to proceed, this card is assigned to an employee!'); // if card is assigned to any employee throw error

  result.is_active = false;
  result.is_returned = true;

  return await result.save();
}

async function deactivateOneById(id) { // TODO - set active to false, set primary false if set && remove from all units
  const result = await Card.findById(id);
  if (!result) throw new error.NotFoundError('Card');

  result.is_active = false;

  return await result.save();
}

async function create(body) {
  const result = new Card(body);
  return await result.save();
}

module.exports = {
  getAll,
  getAllFull,
  getAllUnassigned,
  getAllActive,
  getAllUnactive,
  // getAllPrimary,
  getOneById,
  getOneByIdFull,
  getOneByHexUid,
  updateOneById,
  returnOneById,
  deactivateOneById,
  create,
};