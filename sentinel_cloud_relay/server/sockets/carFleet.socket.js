
function socketHandler(io) {
  const carFleetNamespace = io.of('/car-fleet');

  carFleetNamespace.use((socket, next) => {

    // TODO
    next();

  });


  carFleetNamespace.on('connection', (socket) => {

    // TODO

    // ? - only one endpoint -> send endpoint name as payload.request and payload as payload.payload and handdle callback if needed
    
    // ? - endpoint for each endpoint -> call request dirrectly for each endpoint


    var ip = socket.handshake.address.split('::ffff:');
    console.log(`CarFleet - Client Connected! - ID: ${socket.id}, IP: ${ip[1]}`);

    socket.on('disconnect', () => {
      var ip = socket.handshake.address.split('::ffff:');
      console.log(`CarFleet - Client Disconnected! - ID: ${socket.id}, IP: ${ip[1]}`);
    });
  });

}



export default {
  socketHandler,
}
