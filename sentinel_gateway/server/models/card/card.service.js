import CardModel from './card.model.js';

async function exists(...params) {
  var result = await CardModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await CardModel.find(...params);
  if (!result) throw new Error('No cards found in database!');
  return result;
};

async function getOne(...params) {
  var result = await CardModel.findOne(...params);
  if (!result) throw new Error('Card not found in database!');
  return result;
};

async function modify(id, params) { 
  var result = await CardModel.findOne({_id: id});
  if (!result) throw new Error('Card not found in database!');
  Object.assign(result, params);
  await result.save();
};

async function create(params) {
  var newItem = new CardModel(params);
  await newItem.save();
};

async function remove(id) {
  var result = await CardModel.findOne({_id: id});
  if (!result) throw new Error('Card not found in database!');
  await result.remove();
};

async function checkValidity(id) { // TODO - check
  var currentTimestamp = new Date();
  var result = await CardModel.findOne({_id: id});
  if (!result) throw new Error('Card not found in database!');
  if (result.is_enabled) {
    if (result.has_validity) {
      if (currentTimestamp < result.valid_since) {
        throw new Error('Card is not yet available to use!');
      } else if (currentTimestamp > result.valid_until) {
        throw new Error('Card is expired!');
      } else {
        return true;
      }
    } else {
      return true;
    }
  } else {
    throw new Error('Card is disabled!');
  }
}

export default {
  exists,
  get,
  getOne,
  modify,
  create,
  remove,
  checkValidity,
};
