const express = require('express');
const router = express.Router();
const morgan = require('../helpers/morgan');

router.use(morgan.databaseLoggerHandler);

// Serve all existing routes
// router.use('/', );

// mysql admin panel
// router.use('/mysql', );

// mongo admin panel
// router.use('/mongo', );

// 404 Endpoint Not Found // TODO
router.use('*', (req, res) => res.status(404).send('Database - Not Found - Error'));

module.exports = router;
