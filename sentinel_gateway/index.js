import * as dotenv from 'dotenv';
dotenv.config();
import logSymbols from 'log-symbols';
import https from 'https';
import fs from 'fs';
import express from 'express';
import cors from 'cors';
import helmet from 'helmet';
import { Server } from "socket.io";
import client from './server/controllers/client.controller.js';
import socket from './server/controllers/socket.controller.js';
import { clientLoggerHandler } from './server/controllers/morgan.controller.js';
import { mongoConnect } from './server/controllers/mongo.controller.js';
import { cronSetup } from './server/controllers/cron.controller.js';
import filesRoutes from './server/routes/files.js';
import apiRoutes from './server/routes/api.js';

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

// if crt files provided, make them into pem files, and if chain needed, move chain at the and of cert file to create fullchain
const privateKey  = fs.readFileSync('./certificates/privkey.pem', 'utf8');
const certificate = fs.readFileSync('./certificates/fullchain.pem', 'utf8');

const app = express();
const server = https.createServer({ key: privateKey, cert: certificate }, app);
const io = new Server(server);

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
app.use(helmet()); // Server Security
app.use('/api', apiRoutes); // Backend /api routes
app.use(clientLoggerHandler); // morgan client logger
app.use(client.frontendHostingHandler); // Frontend Hosting - failover route

socket.socketHandler(io);

server.on('error', onError);
server.on('listening', onListening);
server.listen(443);
