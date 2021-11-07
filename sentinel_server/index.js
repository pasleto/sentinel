require('dotenv').config();
const app = require('./server/app');
const pjson = require('./package.json');
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
  var bind = process.env.USE_SSL == 'true' ? `using https on port ${process.env.HTTPS_PORT}` : `using http on port ${process.env.HTTP_PORT}`;
  console.log(logSymbols.info, `Sentinel - Version: ${pjson.version} - © 2021 Tomáš Pásler`);
  console.log(logSymbols.info, `Server is running ${bind}`);
}

if(process.env.USE_SSL == 'true') {
  app.set('port', process.env.HTTPS_PORT);
  var server = https.createServer(httpsOptions,app).listen(process.env.HTTPS_PORT);
  server.on('error', onError);
  server.on('listening', onListening);
} else {
  app.set('port', process.env.HTTP_PORT);
  var server = http.createServer(app).listen(process.env.HTTP_PORT);
  server.on('error', onError);
  server.on('listening', onListening);
}

