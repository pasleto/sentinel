
function socketHandler(io) {
  const cloudRelayNamespace = io.of('/cloud-relay');

  cloudRelayNamespace.use((socket, next) => {

    // TODO - validate sentinel server
    next();
    
  });


  cloudRelayNamespace.on('connection', (socket) => {

    // TODO - here is our sentinel server <- everything should be sent to this exact socket

    global.sentinel = socket;

    var ip = socket.handshake.address.split('::ffff:');
    console.log(`CloudRelay - Client Connected! - ID: ${socket.id}, IP: ${ip[1]}`);

    socket.on('disconnect', () => {
      global.sentinel = null;

      var ip = socket.handshake.address.split('::ffff:');
      console.log(`CloudRelay - Client Disconnected! - ID: ${socket.id}, IP: ${ip[1]}`);
    });
  });

}



export default {
  socketHandler,
}
