// import utils from '../../utils/util.js';
import { userService, departmentService, cardService } from '../../controllers/mongo.controller.js';

// socket can be accessed by this in classic funtion

// ? graphlookup instead of recursive recursion

// async function _getRecursiveParent(id, array) {
//   // array.push(id); // including self
//   var dep = await departmentService.getOne({ _id: id }, 'parent'); // var dep = await departmentService.getOne({ _id: id }, '-createdAt -updatedAt -__v');
//   if (dep.parent) {
//     array.push(dep.parent); // exluding self
//     await _getRecursiveParent(dep.parent, array);
//   }
//   return array;
// }

// async function _getRecursiveUsers(array) {
//   return await Promise.all(array.map(async item => {
//     // item.parents = await _getRecursiveParent(item.department, []); // exluding self
//     item.parents = await _getRecursiveParent(item.department, [item.department]); // including self
//     return item;
//   }));
// }

async function _getRecursiveData(array) {
  return await Promise.all(array.map(async item => {
    // var users = await userService.get(true, { path: 'cards', select: '-user -createdAt -updatedAt -__v', populate: [{ path: 'type', select: '-cards -createdAt -updatedAt -__v' }, { path: 'last_changed_by', select: 'name surname username user_number' }] }, { department: item._id, is_protected: false }, '-password -createdAt -updatedAt -__v');
    // item.users = await _getRecursiveUsers(users);
    // item.parents = await _getRecursiveParent(item._id, []);
    // TODO - card query return -> what logic use
    item.users = await userService.get(true, { path: 'cards', select: '-user -createdAt -updatedAt -__v', populate: [{ path: 'type', select: '-cards -createdAt -updatedAt -__v' }, { path: 'last_changed_by', select: 'name surname username user_number' }] }, { department: item._id, is_protected: false }, '-password -createdAt -updatedAt -__v');
    var childDepartments = await departmentService.getLean({ parent: item._id }, '-createdAt -updatedAt -__v');
    item.departments = await _getRecursiveData(childDepartments);
    return item;
  }));
}

async function getFullList(payload, callback) {
  try {
    var rootDepartments = await departmentService.getLean({ is_root: true }, '-createdAt -updatedAt -__v');
    var output = await _getRecursiveData(rootDepartments);

    callback({
      status: 'OK',
      data: {
        list: output
      }
    });
  } catch (error) {
    callback({
      status: 'NOK',
      data: {
        message: error.message
      }
    });
  }
};

export default {
  getFullList,
}
