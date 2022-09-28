// TODO - maybe split errors into CustomErrors

import UserModel from './user.model.js';
import userInitData from './user.init.js';
import _ from 'underscore';
import { cardService } from '../../controllers/mongo.controller.js';

async function init() {
  _.each(userInitData, async (data) => {
    var itemExist = await UserModel.exists({ user_number: data.user_number });
    if (!itemExist) { 
      var newItem = new UserModel({ 
        user_number: data.user_number,
        name: data.name,
        surname: data.surname,
        username: data.username,
        is_admin: data.is_admin,
        login_enabled: data.login_enabled,
        is_ldap_user: data.is_ldap_user,
        password: data.password,
        is_protected: data.is_protected
      });
      await newItem.save();
    }
  });
};

async function exists(...params) { // TODO - findOne vs find ??
  var result = await UserModel.findOne(...params);
  if (!result) return false;
  return result;
};

async function get(...params) {
  var result = await UserModel.find(...params);
  if (!result) throw new Error('No users found in database!');
  return result;
};

async function getOne(...params) {
  var result = await UserModel.findOne(...params);
  if (!result) throw new Error('User not found in database!');
  return result;
};

async function create(params) {
  var newItem = new UserModel(params);
  await newItem.save();
};

// async function getOnePopulate(params, refs) {
//   var result = await UserModel.findOne(params).populate(refs);
//   if (!result) throw new Error('User not found in database!');
//   return result;
// };

export default {
  init,
  exists,
  get,
  getOne,
  create,
};
