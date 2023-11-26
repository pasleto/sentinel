import loginService from './login.service.js';
import ldapService from './ldap.service.js';
import userService from './user.service.js';
import departmentService from './department.service.js';
import materialstorageService from './materialstorage.service.js';
import carfleetService from './carfleet.service.js';
import settingsService from './settings.service.js';

function socketConnect(socketID) { // TODO - add timestamp - logout/remove socket after some time of disconnecting - need to be handled on frontend aswell
  var socketObj = global.connectedClientApp.find(item => item.socket_id == socketID );
  if(!socketObj) {
    global.connectedClientApp.push({
      socket_id: socketID,
      username: null,
      user_id: null,
      is_logged: false,
      is_connected: true
    });
  } else {
    for (const item of global.connectedClientApp) {
      if (item.socket_id == socketID) {
        is_connected = true;
        break;
      }
    }
  }
};

function getSocket(socketID) {
  var socketObj = global.connectedClientApp.find(item => item.socket_id == socketID );
  if(!socketObj) {
    return null;
  } else {
    return socketObj;
  }
};

// function socketLogin(socketID, username, userID) {
//   for (const item of global.connectedClientApp) {
//     if (item.socket_id == socketID) {
//       item.username = username;
//       item.user_id = userID;
//       item.is_logged = true;
//       break;
//     }
//   }
// };

// function socketLogout(socket, callback) {
//   socket.rooms.forEach(element => {
//     if (element != socket.id) { // ? and any other room which should be still part of
//       socket.leave(element);
//     }
//   });
//   for (const item of global.connectedClientApp) {
//     if (item.socket_id == socket.id) {
//       item.username = null;
//       item.user_id = null;
//       item.is_logged = false;
//       callback({
//         status: 'OK',
//         data: {
//           message: 'Logged out!'
//         }
//       });
//       break;
//     }
//   }
// };

function socketDisconnect(socketID) {
  for (const item of global.connectedClientApp) {
    if (item.socket_id == socketID) {
      item.is_connected = false;
      break;
    }
  }
};

function getAllClientAppSockets(payload, callback) {
  try {
    var disconnected_array = global.connectedClientApp.filter(item => item.is_connected == false && item.is_logged == false);
    var disconnected_logged_array = global.connectedClientApp.filter(item => item.is_connected == false && item.is_logged == true);
    var connected_array = global.connectedClientApp.filter(item => item.is_connected == true && item.is_logged == false);
    var connected_logged_array = global.connectedClientApp.filter(item => item.is_connected == true && item.is_logged == true);
    callback({
      status: 'OK',
      data: {
        disconnected: disconnected_array,
        disconnected_logged: disconnected_logged_array,
        connected: connected_array,
        connected_logged: connected_logged_array
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

function getAllMaterialStorageSockets(payload, callback) {
  try {
    var disconnected_array = global.connectedMaterialStorage.filter(item => item.is_connected == false && item.is_logged == false);
    var disconnected_logged_array = global.connectedMaterialStorage.filter(item => item.is_connected == false && item.is_logged == true);
    var connected_array = global.connectedMaterialStorage.filter(item => item.is_connected == true && item.is_logged == false);
    var connected_logged_array = global.connectedMaterialStorage.filter(item => item.is_connected == true && item.is_logged == true);
    callback({
      status: 'OK',
      data: {
        disconnected: disconnected_array,
        disconnected_logged: disconnected_logged_array,
        connected: connected_array,
        connected_logged: connected_logged_array
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

function getAllAccessControlSockets(payload, callback) {
  try {
    var disconnected_array = global.connectedAccessControl.filter(item => item.is_connected == false && item.is_logged == false);
    var connected_array = global.connectedAccessControl.filter(item => item.is_connected == true && item.is_logged == false);
    callback({
      status: 'OK',
      data: {
        disconnected: disconnected_array,
        connected: connected_array,
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

// function socketRemove(socketID) {
//   global.connectedClientApp = global.connectedClientApp.filter(item => item.socket_id !== socketID);
// };

function socketHandler(io) {
  const clientAppNamespace = io.of('/client-app');

  clientAppNamespace.use((socket, next) => { // ensure the socket has access to the 'client-app' namespace, and then
    // socketConnect(socket.id); // TODO - on connection maybe check if exist and if is logged, and return logged user to frontend | problem with logged on frontend and lost connection - user will be logged in frontend but not in backend

    // console.log(socket.handshake.auth.token);
    // next();

    // TODO - check if token is passed, if is valid, and if expiration is close, send refreshed token, if user is admin then join admin room

    if (socket.handshake.auth.token) {
      // TODO - validate jwt token
      next();
    } else {
      next(); // TODO - for postman dev only
      // next(Error({
      //   status: 'NOK',
      //   data: {
      //     type: 'authorization_error',
      //     message: `No authorization token passed`
      //   }
      // }));
    }
  });

  clientAppNamespace.on('connection', (socket) => {

    var ip = socket.handshake.address.split('::ffff:');
    console.log(`ClientApp - Client Connected! - ID: ${socket.id}, IP: ${ip[1]}`);
    // TODO

    socket.on('app-socket-clients-array', getAllClientAppSockets);
    socket.on('ms-socket-clients-array', getAllMaterialStorageSockets);
    socket.on('ac-socket-clients-array', getAllAccessControlSockets);

    socket.on('login', loginService.getUserData); // TODO

    // socket.on('login-password', loginService.loginPassword);
    // socket.on('logout', loginService.logout); // ? - for clientApp maybe delete the socket aswell on logout, as new socket is handled on login from frontend

    socket.on('ldap-test-connection', ldapService.ldapTestConnection);
    socket.on('ldap-sync-full', ldapService.ldapSyncFull); // TODO
    socket.on('ldap-sync-one', ldapService.ldapSyncOne); // TODO

    socket.on('user-card-search', userService.userCardSearch);
    socket.on('user-card-create', userService.userCardCreate);
    socket.on('user-card-edit', userService.userCardEdit);
    socket.on('user-card-delete', userService.userCardDelete);
    socket.on('user-card-barcode-generate', userService.userCardBarcodeGenerate);

    socket.on('user-get', userService.userGet); // TODO
    socket.on('user-create', userService.userCreate); // TODO
    socket.on('user-edit', userService.userEdit); // TODO
    socket.on('user-delete', userService.userDelete); // TODO

    socket.on('department-get-list', departmentService.getFullList); // TODO

    socket.on('ms-device-get-one', materialstorageService.msDeviceGetOne);
    socket.on('ms-device-get-all', materialstorageService.msDeviceGetAll);
    socket.on('ms-device-create', materialstorageService.msDeviceCreate);
    socket.on('ms-device-edit', materialstorageService.msDeviceEdit); // TODO
    socket.on('ms-device-delete', materialstorageService.msDeviceDelete);

    // socket.on('ms-group-get-one', materialstorageService.msGroupGetOne); // TODO
    // socket.on('ms-group-get-all', materialstorageService.msGroupGetAll); // TODO
    // socket.on('ms-group-create', materialstorageService.msGroupCreate); // TODO
    // socket.on('ms-group-edit', materialstorageService.msGroupEdit); // TODO
    // socket.on('ms-group-delete', materialstorageService.msGroupDelete); // TODO


    socket.on('cf-car-get-one', carfleetService.cfCarGetOne); // TODO
    socket.on('cf-car-get-all', carfleetService.cfCarGetAll); // TODO
    socket.on('cf-car-create', carfleetService.cfCarCreate); // TODO
    socket.on('cf-car-edit', carfleetService.cfCarEdit); // TODO
    socket.on('cf-car-delete', carfleetService.cfCarDelete); // TODO

    socket.on('cf-device-get-one', carfleetService.cfDeviceGetOne); // TODO
    socket.on('cf-device-get-all', carfleetService.cfDeviceGetAll); // TODO
    socket.on('cf-device-create', carfleetService.cfDeviceCreate); // TODO
    socket.on('cf-device-edit', carfleetService.cfDeviceEdit); // TODO
    socket.on('cf-device-delete', carfleetService.cfDeviceDelete); // TODO







    socket.on('settings-get-one', settingsService.settingsGetOne); // TODO


    // socket.on('disconnect', () => socketDisconnect(socket.id)); // TODO
    socket.on('disconnect', () => {
      var ip = socket.handshake.address.split('::ffff:');
      console.log(`ClientApp - Client Disconnected! - ID: ${socket.id}, IP: ${ip[1]}`);

      socketDisconnect(socket.id)
    });
  });
};

export default {
  socketHandler,
  getSocket,
  socketConnect,
  // socketRemove,
  // socketLogin,
  // socketLogout,
  socketDisconnect,
};
