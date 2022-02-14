const express = require('express');
const path = require('path');
const router = express.Router();
const morgan = require('../helpers/morgan');

router.use(morgan.serverLoggerHandler);

// Handle server settings api routes
router.get('/api', function (req, res) { // TODO
  res.status(200).send('Hello Server Api');
});

// Server html page
router.get('/', (req, res) => res.status(200).sendFile(path.join(__dirname, '../views/server.html')));

// 404 Endpoint Not Found // TODO
router.use('*', (req, res) => res.status(404).send('Server - Not Found - Error'));

module.exports = router;