import fs from 'fs';
import axios from 'axios';
import https from 'https';

function socketHandler(io) {
  
  // TODO - this should be cloud-relay server ip
  const cloudRelaySocket = io('https://sentinel-cloud-relay.pasler.org:443/cloud-relay', {
    reconnection: true,
    autoConnect: false,
    // ca: fs.readFileSync('./certificates/_root/rootCA.pem', 'utf8'), // TODO - cloud-relay server CA root - only if not public signed -> eg. private cloud -> but not needed if public ssl cert issuer
  });

  // cloudRelaySocket.io.on('error', (error) => {
  //   console.log('Error connecting to cloud relay: ', error);
  // });

  cloudRelaySocket.on('connect', (socket) => {
    console.log('Connected to cloud relay server!');
  });

  cloudRelaySocket.on('disconnect', (reason) => {
    if (reason === 'io server disconnect') {
      // the disconnection was initiated by the server, you need to reconnect manually
      console.log('Disconnected by remote cloud relay server, trying to reconnect manually!')
      cloudRelaySocket.connect();
    }
    console.log('Disconnected from cloud relay server, trying to reconnect automatically!')
    // else the socket will automatically try to reconnect
  });

  cloudRelaySocket.on('api-call-get', (payload, callback) => {

    payload.headers.host = 'localhost'; // TODO - value which is in the localhost cert's altnames

    axios({
      method: 'get',
      url: `https://localhost:443${payload.path}`, // TODO
      headers: payload.headers,
      data: payload.body,
      httpsAgent: https.Agent({
        ca: fs.readFileSync('./certificates/_root/rootCA.pem', 'utf8'), // TODO - localhost CA root
      }),
    }).then(function (response) {

      // TODO 

      callback({
        status: response.status,
        contentType: response.headers.getContentType(),
        data: response.data
      });

    }).catch(function (error) {

      console.log(error);

      if (error.response.data != null) {
        callback(error.response.data);
      } else {
        callback({ 
          status: 'NOK', 
          data: {
            message: 'Generic error!'
          } 
        });
      }

    });
  });

  cloudRelaySocket.on('api-call-post', (payload, callback) => {
    
    payload.headers.host = 'localhost'; // TODO - value which is in the localhost cert's altnames

    axios({
      method: 'post',
      url: `https://localhost:443${payload.path}`,
      headers: payload.headers,
      data: payload.body,
      httpsAgent: https.Agent({ 
        ca: fs.readFileSync('./certificates/_root/rootCA.pem', 'utf8'), // TODO - cloud-relay server CA root
      }),
    }).then(function (response) {

      // callback(response.data); // TODO

      callback({
        status: response.status,
        contentType: response.headers.getContentType(),
        data: response.data
      });

    }).catch(function (error) {

      if (error.response.data != null) {
        callback(error.response.data);
      } else {
        callback({ 
          status: 'NOK', 
          data: {
            message: 'Generic error!'
          } 
        });
      }

    });
  });

  // TODO

  cloudRelaySocket.on('car-fleet', (payload, callback) => {

    // ? - only this endpoint -> switch based on payload.request and call service function with payload.payload and callback if needed
    
    // ? - endpoint for each endpoint -> call service dirrectly on each endpoint

  });



  cloudRelaySocket.on('api-call-test-post', (payload, callback) => { // TODO

    console.log(payload);

    callback({ 
      status: 'OK', 
      data: {
        message: 'OK' 
      }
    });

    // callback({ 
    //   status: 'NOK', 
    //   data: {
    //     message: 'NOT OK' 
    //   }
    // });
  });



  cloudRelaySocket.connect();

};

export default { 
  socketHandler,
};
