import 'dotenv/config';
import logSymbols from 'log-symbols';
import https from 'https';
import fs from 'fs';
import express from 'express';
import cors from 'cors';
import helmet from 'helmet';
import { Server } from "socket.io";
import * as client from './server/utils/client.js';
import * as socket from './server/sockets/socket.js';
import { clientLoggerHandler } from './server/utils/morgan.js';
import { mongoConnect } from './server/utils/mongo.js';
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

const privateKey  = fs.readFileSync('./certificates/CA.key', 'utf8');
const certificate = fs.readFileSync('./certificates/CA.pem', 'utf8');

const app = express();
const server = https.createServer({ key: privateKey, cert: certificate }, app);
const io = new Server(server);

mongoConnect(); // mongo database connection

app.use(cors());
app.use(express.json());
app.use(client.compressHandler); // frontend compression handler
app.use(client.frontendStaticFilesHandler); // frontend static files hosting

// app.use((req, res, next) => {
//   req.io = io;
//   next();
// });
app.use(socket.socketApiHandler(io));

app.use('/files', filesRoutes); // File Server Hosting
app.use(helmet()); // Server Security
app.use('/api', apiRoutes); // Backend /api routes
app.use(clientLoggerHandler); // morgan client logger
app.use(client.frontendHostingHandler); // Frontend Hosting - failover route

socket.socketRoutesHandler(io);

server.on('error', onError);
server.on('listening', onListening);
server.listen(443);
