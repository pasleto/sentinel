import DepartmentModel from './department.model.js';
import departmentInitData from './department.init.js';
import _ from 'underscore';

async function init() {
  _.each(departmentInitData, async (data) => {
    var itemExist = await DepartmentModel.exists({ department_number: data.department_number });
    if (!itemExist) { 
      var newItem = new DepartmentModel({ 
        department_number: data.department_number,
        name: data.name,
        is_root: data.is_root,
        can_have_child: data.can_have_child,
        description: data.description,
        ldap_sync: data.ldap_sync,
        is_protected: data.is_protected
      });
      await newItem.save();
    }
  });
};

async function exists(...params) {
  var result = await DepartmentModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await DepartmentModel.find(...params);
  if (!result) throw new Error('No department found in database!');
  return result;
};

async function getOne(params) {
  var result = await DepartmentModel.findOne(params);
  if (!result) throw new Error('Department not found in database!');
  return result;
};

async function create(params) {
  var newItem = new DepartmentModel(params);
  await newItem.save();
};

export default {
  init,
  exists,
  get,
  getOne,
  create,
};
