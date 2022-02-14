const error = require('../../../helpers/error');
const mongo = require('../../../helpers/mongo');
const Employee = mongo.Employee;

async function getAll() {
  return await Employee.find();
}

async function getOneById(id) {
  const result = await Employee.findById(id);
  if (!result) throw new error.NotFoundError('Employee');
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