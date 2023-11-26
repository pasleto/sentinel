import _ from 'underscore';
import jwt from 'jsonwebtoken';
import { userService, settingService } from './mongo.controller.js';

async function clientAppGetUserData(jwtToken, callback) {
  try {

    console.log('JWT TOKEN: ' + jwtToken);

    var hash_jwt = await settingService.getOne({ scope: 'general', name: 'hash_jwt' });

    var decodedToken = jwt.verify(jwtToken, hash_jwt.value);
    console.log('USER ID: ' + decodedToken.user_id);

    var user = await userService.getOne(false, {
      path: 'cards',
      select: '-user -createdAt -updatedAt -__v',
      populate: [
        {
          path: 'type',
          select: '-cards -createdAt -updatedAt -__v'
        },
        {
          path: 'last_changed_by',
          select: 'name surname username user_number'
        }
      ]
    }, { _id: decodedToken.user_id });
    console.log(user); // TODO

    callback({
      status: 'OK',
      data: {
        user: _.omit(user.toObject(), 'password', 'createdAt', 'updatedAt', '__v')  // ? - check what else omit
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
}



export default {
  clientAppGetUserData,
};