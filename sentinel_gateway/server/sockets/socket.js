
// import xxx

function socketApiHandler(io) {
  return (req, res, next) => {
    req.io = io;
    next();
  };
};

function socketRoutesHandler(io) {
  // console.log(io);

  io.on("connection", (socket) => {
    var ip = socket.handshake.address.split("::ffff:");
    console.log(`Client Connected! - ID: ${socket.id}, IP: ${ip[1]}`);
    // console.log(`ID: ${socket.id}, Host: ${socket.handshake.headers.host}, IP: ${socket.handshake.address}`);
  
    // socket.on("disconnecting", () => {
    //   console.log('Client Disconnecting!');
    //   console.log(`ID: ${socket.id}, Host: ${socket.handshake.headers.host}, IP: ${socket.handshake.address}`);
    // });
  
    socket.on("disconnect", () => {
      var ip = socket.handshake.address.split("::ffff:");
      console.log(`Client Disconnected! - ID: ${socket.id}, IP: ${ip[1]}`);
      // console.log(`ID: ${socket.id}, Host: ${socket.handshake.headers.host}, IP: ${socket.handshake.address}`);
    });


    socket.on("access-control/log/reader", (data) => {
      console.log('access-control/log/reader');
      console.log(data);
    });
    socket.on("access-control/log/button", (data) => {
      console.log('access-control/log/button');
      console.log(data);
    });
    socket.on("access-control/log/door", (data) => {
      console.log('access-control/log/door');
      console.log(data);
    });

  });

};

export { 
  socketApiHandler,
  socketRoutesHandler,
};
