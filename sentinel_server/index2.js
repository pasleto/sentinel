require('dotenv').config();
const app = require('./server/app2');
const logSymbols = require('log-symbols');
const http = require('http');
const https = require('https');
const fs = require('fs');
const privateKey  = fs.readFileSync('./certificates/LocalhostCA.key', 'utf8');
const certificate = fs.readFileSync('./certificates/LocalhostCA.pem', 'utf8');
const httpsOptions = {key: privateKey, cert: certificate};


function onError (error) {
  if (error.syscall !== 'listen') { throw error; }
  switch (error.code) {
    case 'EACCES':
      console.error(logSymbols.error, 'Elevated privileges required \n');
      process.exit(1);
      break;
    case 'EADDRINUSE':
      console.error(logSymbols.error, 'Port ' + error.port + ' is already in use \n');
      process.exit(1);
      break;
    default:
      throw error;
  }
}

function onBackendListening () {
  console.log(logSymbols.info, `Backend server is running`);
}

function onFrontendListening () {
  console.log(logSymbols.info, `Frontend server is running`);
}


var backendServer = https.createServer(httpsOptions,app.backend).listen(8443);
backendServer.on('error', onError);
backendServer.on('listening', onBackendListening);

if(process.env.TEST_SSL == 'true') {
  var frontendServer = https.createServer(httpsOptions,app.frontend).listen(443);
  frontendServer.on('error', onError);
  frontendServer.on('listening', onFrontendListening);
} else {
  var frontendServer = http.createServer(app.frontend).listen(80);
  frontendServer.on('error', onError);
  frontendServer.on('listening', onFrontendListening);
}

