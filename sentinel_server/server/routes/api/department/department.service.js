const error = require('../../../helpers/error');
const mongo = require('../../../helpers/mongo');
const Department = mongo.Department;

async function getAll() {
  return await Department.find();
}

async function getOneById(id) {
  const result = await Department.findById(id);
  if (!result) throw new error.NotFoundError('Department');
  return result;
}

async function updateOneById(id, body) {

}

async function deleteOneById(id) {

}

async function deleteAll() {

}

async function create() {

}

module.exports = {
  getAll,
  getOneById,
  updateOneById,
  deleteOneById,  
  deleteAll,
  create,
};