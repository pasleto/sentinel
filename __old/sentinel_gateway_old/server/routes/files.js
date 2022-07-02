const express = require('express');
const serveIndex = require('serve-index');
const path = require('path');
const router = express.Router();
const morgan = require('../helpers/morgan');

const filesDirectoryHandler = serveIndex(path.join(__dirname, '../../files/web_share'), {'icons': true, 'view': 'tiles', 'hidden': true});
const filesStaticHandler = express.static(path.join(__dirname, '../../files/web_share'));

router.use(morgan.filesLoggerHandler);

// Serve all existing routes
router.use('/', filesStaticHandler, filesDirectoryHandler);

// 404 Endpoint Not Found // TODO
router.use('*', (req, res) => res.status(404).send('Files - Not Found - Error'));

module.exports = router;
