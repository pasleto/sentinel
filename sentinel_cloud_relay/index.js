import * as dotenv from 'dotenv';
dotenv.config();
import logSymbols from 'log-symbols';
import https from 'https';
import http from 'http';
import fs from 'fs';
import express from 'express';
import cors from 'cors';
import { Server } from "socket.io";
import socket from './server/controllers/socket.controller.js';
import api from './server/controllers/api.controller.js';

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

function onListeningHttps () {
  console.log(logSymbols.info, `HTTPS server is running: https://localhost:443/`);
}

function onListeningHttp () {
  console.log(logSymbols.info, `HTTP server is running: http://localhost:80/`);
}

// if crt files provided, make them into pem files, and if chain needed, move chain at the and of cert file to create fullchain
const privateKey  = fs.readFileSync('./certificates/privkey.pem', 'utf8');
const certificate = fs.readFileSync('./certificates/fullchain.pem', 'utf8');

const app = express();
const server_https = https.createServer({ key: privateKey, cert: certificate }, app);
const server_http = http.createServer(app);

const io = new Server();
io.attach(server_https); // TODO - sentinel connections needs to be on SSL
io.attach(server_http); // TODO - iot modules might not support SSL -> maybe create separate IO instance with separated declaration -> server will handle redirection to SSL one to sentinel server connection

global.io = io;
global.sentinel = null;

app.use(cors());
app.use(express.json());
app.use('/', api);
socket.socketHandler(io);

server_https.on('error', onError);
server_https.on('listening', onListeningHttps);
server_https.listen(8443);

server_http.on('error', onError);
server_http.on('listening', onListeningHttp);
server_http.listen(8080);