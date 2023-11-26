import loginService from "./login.service.js";

function socketConnect(socketID) { // TODO - add timestamp - remove socket after some time of disconnecting - need to be handled on frontend aswell
  var socketObj = global.connectedMaterialStorage.find(item => item.socket_id == socketID );
  if(!socketObj) {
    global.connectedMaterialStorage.push({
      socket_id: socketID,
      username: null,
      user_id: null,
      is_logged: false,
      is_connected: true
    });
  } else {
    for (const item of global.connectedMaterialStorage) {
      if (item.socket_id == socketID) {
        is_connected = true;
        break;
      }
    }
  }
};

function getSocket(socketID) {
  var socketObj = global.connectedMaterialStorage.find(item => item.socket_id == socketID );
  if(!socketObj) {
    return null;
  } else {
    return socketObj;
  }
};

function socketLogin(socketID, username, userID) {
  for (const item of global.connectedMaterialStorage) {
    if (item.socket_id == socketID) {
      item.username = username;
      item.user_id = userID;
      item.is_logged = true;
      break;
    }
  }
};

function socketLogout(socket, callback) {
  socket.rooms.forEach(element => {
    if (element != socket.id) { // ? and any other room which should be still part of
      socket.leave(element);
    }
  });
  for (const item of global.connectedMaterialStorage) {
    if (item.socket_id == socket.id) {
      item.username = null;
      item.user_id = null;
      item.is_logged = false;
      callback({
        status: 'OK',
        data: {
          message: 'Logged out!'
        }
      });
      break;
    }
  }
};

function socketDisconnect(socketID) {
  for (const item of global.connectedMaterialStorage) {
    if (item.socket_id == socketID) {
      item.is_connected = false;
      break;
    }
  }
};

// function socketRemove(socketID) {
//   global.connectedMaterialStorage = global.connectedMaterialStorage.filter(item => item.socket_id !== socketID);
// };

// TODO - set header socket.handshake.headers.device with deviceID

function socketHandler(io) {
  const materialStorageNamespace = io.of('/material-storage');

  materialStorageNamespace.use((socket, next) => {
    // ensure the socket has access to the 'material-storage' namespace, and then
    socketConnect(socket.id);
    next();
  });

  materialStorageNamespace.on('connection', (socket) => {
                    console.log(`MaterialStorage - Client Connected! - ID: ${socket.id}, IP: ${socket.handshake.address.split('::ffff:')[1]}`);



    socket.on('login-password', loginService.loginPassword);
    socket.on('login-card', loginService.loginCard);
    socket.on('login-barcode', loginService.loginBarcode);
    socket.on('logout', loginService.logout);



    socket.on('disconnect', () => {
                    console.log(`MaterialStorage - Client Disconnected! - ID: ${socket.id}, IP: ${socket.handshake.address.split('::ffff:')[1]}`);

      socketDisconnect(socket.id);
    });
  });
};

export default {
  socketHandler,
  getSocket,
  socketConnect,
  socketDisconnect,
  socketLogin,
  socketLogout,
};
