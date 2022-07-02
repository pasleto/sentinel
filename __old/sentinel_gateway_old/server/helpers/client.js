const compression = require('compression');
const express = require('express');
const router = express.Router();
const path = require('path');

const shouldCompress = (req, res) => {
  if (req.headers['x-no-compression']) { return false; }
  return compression.filter(req, res);
};

const compressHandler = compression({ filter: shouldCompress, threshold: 0 });
const frontendStaticFilesHandler = express.static(path.join(__dirname, '../../client'));
const frontendHostingHandler = router.use('*', (req, res) => { res.sendFile(path.join(__dirname, '../../client/index.html')); });

module.exports = {
  compressHandler,
  frontendStaticFilesHandler,
  frontendHostingHandler
};