const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const client = require('./helpers/client');
const morgan = require('./helpers/morgan');
const mongo = require('./helpers/mongo');
const backend = express();
const frontend = express();

mongo.connect(); // mongo database connection

backend.use(cors());
backend.use(express.json());
backend.use(helmet()); // Server Security
backend.use('/api', require('./routes/api')); // Backend /api routes

frontend.use(cors());
frontend.use(express.json());
frontend.use(client.compressHandler); // frontend compression handler
frontend.use(client.frontendStaticFilesHandler); // frontend static files hosting
frontend.use('/files', require('./routes/files')); // File Server Hosting
frontend.use('/server', require('./routes/server')); // Server Side Web Page and API Hosting
frontend.use('/database', require('./routes/database')); // Database Web View Hosting
frontend.use(helmet()); // Server Security
frontend.use(morgan.clientLoggerHandler); // morgan client logger
frontend.use(client.frontendHostingHandler); // Frontend Hosting - failover route

module.exports = {
  backend,
  frontend,
};