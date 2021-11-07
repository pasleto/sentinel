const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const client = require('./helpers/client');
const morgan = require('./helpers/morgan');
const mongo = require('./helpers/mongo');
const app = express();

mongo.connect(); // mongo database connection

app.use(cors());
app.use(express.json());
app.use(client.compressHandler); // frontend compression handler
app.use(client.frontendStaticFilesHandler); // frontend static files hosting
app.use('/files', require('./routes/files')); // File Server Hosting
app.use('/server', require('./routes/server')); // Server Side Web Page and API Hosting
app.use('/database', require('./routes/database')); // Database Web View Hosting
app.use(helmet()); // Server Security
app.use('/api', require('./routes/api')); // Backend /api routes
app.use(morgan.clientLoggerHandler); // morgan client logger
app.use(client.frontendHostingHandler); // Frontend Hosting - failover route

module.exports = app;
