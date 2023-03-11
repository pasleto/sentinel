import { settingService } from '../../controllers/mongo.controller.js';

// socket can be accessed by this in classic funtion

async function settingsGetOne(payload, callback) { // TODO
  try {
    var settingExist = await settingService.getOne({ scope: payload.scope, name: payload.name }); // TODO - await settingService.getOne({scope: 'ldap', name: 'group_mqtt_dn'});
    // await deviceExist.populate({
    //   path: 'groups',
    //   select: '-supplies -devices -createdAt -updatedAt -__v'
    // });
    callback({ 
      status: 'OK', 
      data: {
        device: settingExist
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
  settingsGetOne,
}
