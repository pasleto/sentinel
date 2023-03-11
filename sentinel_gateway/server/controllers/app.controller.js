import logSymbols from 'log-symbols';
import express from 'express';
import cors from 'cors';
import https from 'https';
import fs from 'fs';
import { Server } from 'socket.io';
import * as net from 'net';
import * as tls from 'tls';
import client from './client.controller.js';
import socket from './socket.controller.js';
import mqtt from './mqtt.controller.js';
import NTPServer from './ntp.controller.js';
import { mongoConnect, mongoInit } from './mongo.controller.js';
import { cronSetup } from './cron.controller.js';
import ldap from './ldap.controller.js';
import filesRoutes from '../routes/files.js';
import apiRoutes from '../routes/api.js';
import utils from '../utils/util.js';

function _onError (error) {
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

function _getCertificateFiles() { // if crt files provided, make them into pem files, and if chain needed, move chain at the and of cert file to create fullchain
  try {
    const cert_files = {
      key: fs.readFileSync('./certificates/privkey.pem', 'utf8'),
      cert: fs.readFileSync('./certificates/fullchain.pem', 'utf8')
    }
    return cert_files;
  } catch (error) {
    if (error.code === 'ENOENT') {
      return null;
    } else {
      throw error;
    }
  }
}

async function _getGeneratedCertificate() {
  // var client_cert_pem = await utils.generateClientCertificate('STL-AC-TEST-X'); // ? - test
  // console.log(client_cert_pem); // ? - test
  var server_cert_pem = await utils.generateServerCertificate();
  // console.log(server_cert_pem.certificate); // ? - test
  const cert_files = {
    key: server_cert_pem.privateKey,
    cert: server_cert_pem.certificate
  }
  return cert_files;
}

async function _init() {
  var server_tls = {};
  var mqtt_tls = {};
  const files_cert = _getCertificateFiles();
  const generated_cert = await _getGeneratedCertificate();

        var root_ca = await utils.getRootCertificate(); // ? - test
        console.log(root_ca); // ? - test

  if (files_cert) { // if certificate files provided, use them for server
    server_tls = files_cert;
  } else { // otherwise use self signed certificate
    server_tls = generated_cert;
  }
  mqtt_tls = generated_cert; // use self signed certificate
  // mqtt_tls = files_cert;
  // ? + client certificate validation ?

  const app = express();
  const appServer = https.createServer(server_tls, app);
  const io = new Server(appServer);
  // const io = new Server();
  // io.attach(server_https);
  // io.attach(server_http);
  const aedesServer = mqtt.handle();
  const mqttServer = net.createServer(aedesServer.handle); // TODO - remove
  const mqttsServer = tls.createServer(mqtt_tls, aedesServer.handle);
  const ntpServer = new NTPServer();
  ntpServer.handle();

  global.mqtt = aedesServer;
  global.io = io;
  global.serverRebootRequired = false; // ?
  global.connectedClientApp = []; // ?
  global.connectedAccessControl = []; // ?
  global.connectedMaterialStorage = []; // ?

  app.use(cors());
  app.use(express.json());
  app.use('/files', filesRoutes); // File Server Hosting
  // app.use(helmet()); // Server Security // ? - some headers make flutter crash after http call
  app.use('/api', apiRoutes); // Backend /api routes
  app.use(client.frontendHostingHandler); // Frontend Hosting - failover route

  socket.socketHandler(io);

  appServer.on('error', _onError);
  appServer.on('listening', () => console.log(logSymbols.info, `[SERVER] Listening on: ${appServer.address().address}:${appServer.address().port}`));
  appServer.listen(443, process.env.APP_IP || '0.0.0.0');

        mqttServer.on('error', _onError); // TODO - remove
        mqttServer.on('listening', () => console.log(logSymbols.info, `[MQTT] Listening on: ${mqttServer.address().address}:${mqttServer.address().port}`)); // TODO - remove
        mqttServer.listen(1883, process.env.MQTT_IP || '0.0.0.0'); // TODO - remove

  mqttsServer.on('error', _onError);
  mqttsServer.on('listening', () => console.log(logSymbols.info, `[MQTTS] Listening on: ${mqttsServer.address().address}:${mqttsServer.address().port}`));
  mqttsServer.listen(8883, process.env.MQTTS_IP || '0.0.0.0');

  ntpServer.onError(_onError);
  ntpServer.onListening(() => console.log(logSymbols.info, `[NTP] Listening on: ${ntpServer.address().address}:${ntpServer.address().port}`));
  ntpServer.listen(123, process.env.NTP_IP || '0.0.0.0');
}

async function init() {
  utils.loadCertificates();
  await mongoConnect();
  await mongoInit();
  await utils.initServerProxy();
  await ldap.connectionTest();
  await _init();
  cronSetup();
}

export default {
  init
}