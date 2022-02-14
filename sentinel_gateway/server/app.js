const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const client = require('./helpers/client');
const morgan = require('./helpers/morgan');
const mongo = require('./helpers/mongo');

const server = express();
const backend = express();
const frontend = express();

mongo.connect(); // mongo database connection

if(process.env.USE_SSL == 'true') {
  server.use(cors());
  server.use(express.json());
  server.use(client.compressHandler); // frontend compression handler
  server.use(client.frontendStaticFilesHandler); // frontend static files hosting
  server.use('/files', require('./routes/files')); // File Server Hosting
  server.use('/server', require('./routes/server')); // Server Side Web Page and API Hosting
  server.use('/database', require('./routes/database')); // Database Web View Hosting
  server.use(helmet()); // Server Security
  server.use('/api', require('./routes/api')); // Backend /api routes
  server.use(morgan.clientLoggerHandler); // morgan client logger
  server.use(client.frontendHostingHandler); // Frontend Hosting - failover route

  module.exports = {
    server,
  };
} else {
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
}

