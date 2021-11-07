require('dotenv').config();
const app = require('./server/app3');
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

function onListening () {
  console.log(logSymbols.info, `Server is running: https://localhost:443/`);
}

function onBackendListening () {
  console.log(logSymbols.info, `Backend server is running: https://localhost:443/`);
}

function onFrontendListening () {
  console.log(logSymbols.info, `Frontend server is running: https://localhost:80/`);
}

if(process.env.TEST_SSL == 'true') {
  var server = https.createServer(httpsOptions,app.server).listen(443);
  server.on('error', onError);
  server.on('listening', onListening);
} else {
  var backendServer = https.createServer(httpsOptions,app.backend).listen(443);
  backendServer.on('error', onError);
  backendServer.on('listening', onBackendListening);
  var frontendServer = http.createServer(app.frontend).listen(80);
  frontendServer.on('error', onError);
  frontendServer.on('listening', onFrontendListening);
}

