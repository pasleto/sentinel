
function socketHandler(io) {
  const clientAppNamespace = io.of('/client-app');

  clientAppNamespace.use((socket, next) => {

    // TODO
    next();

  });


  clientAppNamespace.on('connection', (socket) => {

    // TODO

    var ip = socket.handshake.address.split('::ffff:');
    console.log(`ClientApp - Client Connected! - ID: ${socket.id}, IP: ${ip[1]}`);

    socket.on('disconnect', () => {
      var ip = socket.handshake.address.split('::ffff:');
      console.log(`ClientApp - Client Disconnected! - ID: ${socket.id}, IP: ${ip[1]}`);
    });
  });

}



export default {
  socketHandler,
}
