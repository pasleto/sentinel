const express = require('express');
const router = express.Router();
const morgan = require('../helpers/morgan');
// const mongo_express = require('mongo-express/lib/middleware');
// const mongo_express_config = require('../helpers/mongo_express_config');

router.use(morgan.databaseLoggerHandler);

// Serve mongo express
// router.use('/', mongo_express(mongo_express_config));

// 404 Endpoint Not Found // TODO
router.use('*', (req, res) => res.status(404).send('Database - Not Found - Error'));

module.exports = router;
