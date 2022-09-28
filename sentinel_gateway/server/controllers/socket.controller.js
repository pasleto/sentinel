
import clientApp from '../sockets/clientApp/clientApp.socket.js';
import materialStorage from '../sockets/materialStorage/materialStorage.socket.js';
import accessControl from '../sockets/accessControl/accessControl.socket.js';

function socketHandler(io) {
  clientApp.socketHandler(io);
  materialStorage.socketHandler(io);
  accessControl.socketHandler(io);
};

export default { 
  socketHandler,
};
