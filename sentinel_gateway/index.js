import * as dotenv from 'dotenv';
dotenv.config();
import pjson from './package.json' assert { type: 'json' };
import logSymbols from 'log-symbols';
import https from 'https';
import fs from 'fs';
import express from 'express';
import cors from 'cors';
// import helmet from 'helmet';
import { Server } from "socket.io";
import client from './server/controllers/client.controller.js';
import socket from './server/controllers/socket.controller.js';
import { clientLoggerHandler } from './server/controllers/morgan.controller.js';
import { mongoConnect } from './server/controllers/mongo.controller.js';
import { cronSetup } from './server/controllers/cron.controller.js';
import filesRoutes from './server/routes/files.js';
import apiRoutes from './server/routes/api.js';

// import aedes from 'aedes';
import mqtt from './server/controllers/mqtt.controller.js';
import * as net from 'net';

// import { io as socketIOClient } from 'socket.io-client';
// import cloud from './server/controllers/cloud.controller.js';

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
  console.log(logSymbols.info, `[Server] Server is running: https://localhost:${server.address().port}/`);
}

function onMqttListening () {
  console.log(logSymbols.info, `[Mqtt] Server is running: mqtt://localhost:${mqttServer.address().port}/`);
}

// if crt files provided, make them into pem files, and if chain needed, move chain at the and of cert file to create fullchain
const privateKey  = fs.readFileSync('./certificates/privkey.pem', 'utf8');
const certificate = fs.readFileSync('./certificates/fullchain.pem', 'utf8');

const app = express();
const server = https.createServer({ key: privateKey, cert: certificate }, app);
const io = new Server(server);
const aedesServer = mqtt.server();
const mqttServer = net.createServer(aedesServer.handle);

global.mqtt = aedesServer;
global.io = io;
global.serverRebootRequired = false;
global.connectedClientApp = [];
global.connectedAccessControl = [];
global.connectedMaterialStorage = [];

mongoConnect(); // mongo database connection
cronSetup(); // cron jobs

app.use(cors());
app.use(express.json());
app.use(client.compressHandler); // frontend compression handler
app.use(client.frontendStaticFilesHandler); // frontend static files hosting
app.use('/files', filesRoutes); // File Server Hosting
// app.use(helmet()); // Server Security // TODO - some headers make flutter crash after http call
app.use('/api', apiRoutes); // Backend /api routes
app.use(clientLoggerHandler); // morgan client logger
app.use(client.frontendHostingHandler); // Frontend Hosting - failover route

socket.socketHandler(io);

// cloud.socketHandler(socketIOClient); // TODO - cloud relay test

server.on('error', onError);
server.on('listening', onListening);
server.listen(443);

mqttServer.on('error', onError);
mqttServer.on('listening', onMqttListening);
mqttServer.listen(1883);

console.log(logSymbols.info, `----------------------------------------------------------------------------------------------------`);
console.log(logSymbols.info, `[${pjson.name}] ${pjson.description} - Version: ${pjson.version} | ${pjson.author.name} (${pjson.author.url})`);
console.log(logSymbols.info, `----------------------------------------------------------------------------------------------------`);