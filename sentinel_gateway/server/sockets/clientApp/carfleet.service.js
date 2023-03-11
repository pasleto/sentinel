import clientApp from './clientApp.socket.js';
import { cfDeviceService, cfCarService } from '../../controllers/mongo.controller.js';

// socket can be accessed by this in classic funtion

async function cfCarGetAll(payload, callback) { // TODO
  try {
    var carList = await cfCarService.get(false, false, {}, '-createdAt -updatedAt -__v'); // TODO - populate device ?
    callback({ 
      status: 'OK', 
      data: {
        cars: carList
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

async function cfCarGetOne(payload, callback) { // TODO
  try {
    var carExist = await cfCarService.getOne(false, false, { _id: payload.id }); // TODO - populate device ?
    // await deviceExist.populate({
    //   path: 'groups',
    //   select: '-supplies -devices -createdAt -updatedAt -__v'
    // });
    callback({ 
      status: 'OK', 
      data: {
        car: carExist
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

async function cfCarCreate(payload, callback) { // TODO
  try {
     // TODO

    // await msDeviceService.create({
    //   name: payload.name,
    //   device_id: payload.device_id,
    //   hostname: payload.hostname,
    //   mac_address: payload.mac_address,
    //   ...(payload.description && payload.description != null && payload.description != undefined && payload.description != "") && { description: payload.description },
    //   ...(payload.ip_address && payload.ip_address != null && payload.ip_address != undefined && payload.ip_address != "") && { ip_address: payload.ip_address }
    // });
    callback({ 
      status: 'OK', 
      data: {
        message: 'Car Created'
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

async function cfCarEdit(payload, callback) { // TODO
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

async function cfCarDelete(payload, callback) { // TODO - !!!
  try {
    await cfCarService.remove(payload.id);
    callback({ 
      status: 'OK', 
      data: {
        message: 'Car Deleted'
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

async function cfDeviceGetAll(payload, callback) { // TODO
  try {
    var deviceList = await cfDeviceService.get(false, false, {}, '-createdAt -updatedAt -__v'); // TODO - populate car ?
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

async function cfDeviceGetOne(payload, callback) { // TODO
  try {
    var deviceExist = await cfDeviceService.getOne(false, false, { _id: payload.id }); // TODO - populate car ?
    // await deviceExist.populate({
    //   path: 'groups',
    //   select: '-supplies -devices -createdAt -updatedAt -__v'
    // });
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

async function cfDeviceCreate(payload, callback) { // TODO
  try {
     // TODO

    // await msDeviceService.create({
    //   name: payload.name,
    //   device_id: payload.device_id,
    //   hostname: payload.hostname,
    //   mac_address: payload.mac_address,
    //   ...(payload.description && payload.description != null && payload.description != undefined && payload.description != "") && { description: payload.description },
    //   ...(payload.ip_address && payload.ip_address != null && payload.ip_address != undefined && payload.ip_address != "") && { ip_address: payload.ip_address }
    // });
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

async function cfDeviceEdit(payload, callback) { // TODO
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

async function cfDeviceDelete(payload, callback) { // TODO - if device has car assigned
  try {
    await cfDeviceService.remove(payload.id);
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
  cfCarGetOne,
  cfCarGetAll,
  cfCarCreate,
  cfCarEdit,
  cfCarDelete,
  cfDeviceGetOne,
  cfDeviceGetAll,
  cfDeviceCreate,
  cfDeviceEdit,
  cfDeviceDelete,
}
