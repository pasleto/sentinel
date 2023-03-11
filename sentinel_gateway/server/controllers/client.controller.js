import compression from 'compression';
import express from 'express';
import { resolve } from 'path';
import { clientLoggerHandler } from './logger.controller.js';

const router = express.Router();

const shouldCompress = (req, res) => {
  if (req.headers['x-no-compression']) { return false; }
  return compression.filter(req, res);
};

const compressHandler = compression({ filter: shouldCompress, threshold: 0 });
const frontendStaticFilesHandler = express.static(resolve('client'));

router.use(compressHandler); // frontend compression handler
router.use(frontendStaticFilesHandler); // frontend static files hosting
router.use(clientLoggerHandler); // morgan client logger

const frontendHostingHandler = router.use('*', (req, res) => { res.sendFile(resolve('client/index.html')); });

export default {
  // compressHandler,
  // frontendStaticFilesHandler,
  frontendHostingHandler
};
