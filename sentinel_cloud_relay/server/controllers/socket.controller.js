import cloudRelay from '../sockets/cloudRelay.socket.js';
import clientApp from '../sockets/clientApp.socket.js';
import carFleet from '../sockets/carFleet.socket.js';

function socketHandler(io) {
  cloudRelay.socketHandler(io);
  clientApp.socketHandler(io);
  carFleet.socketHandler(io);
};

export default { 
  socketHandler,
};
