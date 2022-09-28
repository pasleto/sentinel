
function socketAdd(socketID) {

};

function getSocket(socketID) {

};

function socketLogin(socketID, username) {

};

function socketLogout(socketID) {

};

function socketRemove(socketID) {

};

function socketHandler(io) { // TODO
  const accessControlNamespace = io.of('/access-control');

  accessControlNamespace.use((socket, next) => {
    // ensure the socket has access to the 'access-control' namespace, and then
    next();
  });

  accessControlNamespace.on('connection', (socket) => {
    var ip = socket.handshake.address.split('::ffff:');
    console.log(`AccessControl - Client Connected! - ID: ${socket.id}, IP: ${ip[1]}`);
    // console.log(`ID: ${socket.id}, Host: ${socket.handshake.headers.host}, IP: ${socket.handshake.address}`);
  
    socket.on('disconnect', () => {
      var ip = socket.handshake.address.split('::ffff:');
      console.log(`AccessControl - Client Disconnected! - ID: ${socket.id}, IP: ${ip[1]}`);
      // console.log(`ID: ${socket.id}, Host: ${socket.handshake.headers.host}, IP: ${socket.handshake.address}`);
    });
  });
};

export default {
  socketHandler,
};
