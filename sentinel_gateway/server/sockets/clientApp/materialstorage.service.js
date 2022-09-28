import clientApp from './clientApp.socket.js';
import { msDeviceService } from '../../controllers/mongo.controller.js';

// socket can be accessed by this in classic funtion

async function msDeviceGetAll(payload, callback) {
  try {
    var deviceList = await msDeviceService.get({}, '-createdAt -updatedAt -__v');
    callback({ 
      status: 'OK', 
      data: {
        devices: deviceList
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

async function msDeviceGetOne(payload, callback) {
  try {
    var deviceExist = await msDeviceService.getOne({ _id: payload.id });
    await deviceExist.populate({
      path: 'groups',
      select: '-supplies -devices -createdAt -updatedAt -__v'
    });
    callback({ 
      status: 'OK', 
      data: {
        device: deviceExist
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

async function msDeviceCreate(payload, callback) {
  try {
    await msDeviceService.create({
      name: payload.name,
      device_id: payload.device_id,
      hostname: payload.hostname,
      mac_address: payload.mac_address,
      ...(payload.description && payload.description != null && payload.description != undefined && payload.description != "") && { description: payload.description },
      ...(payload.ip_address && payload.ip_address != null && payload.ip_address != undefined && payload.ip_address != "") && { ip_address: payload.ip_address }
    });
    callback({ 
      status: 'OK', 
      data: {
        message: 'Device Created'
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

async function msDeviceEdit(payload, callback) { // TODO
  try {

    // TODO
    
    callback({ 
      status: 'OK', 
      data: {
        message: 'TODO'
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

async function msDeviceDelete(payload, callback) { // TODO - if device has group assigned, on delete remove reference from the group -> remove hook
  try {
    await msDeviceService.remove(payload.id);
    callback({ 
      status: 'OK', 
      data: {
        message: 'Device Deleted'
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
  msDeviceGetOne,
  msDeviceGetAll,
  msDeviceCreate,
  msDeviceEdit,
  msDeviceDelete,
}
