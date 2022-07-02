import express from 'express';
import serveIndex from 'serve-index';
import { resolve } from 'path';
import * as morgan from '../utils/morgan.js';

const router = express.Router();

const filesDirectoryHandler = serveIndex(resolve('files/web_share'), {'icons': true, 'view': 'tiles', 'hidden': true});
const filesStaticHandler = express.static(resolve('files/web_share'));

router.use(morgan.filesLoggerHandler);

// Serve all existing routes
router.use('/', filesStaticHandler, filesDirectoryHandler);

// Endpoint Not Found // TODO
// router.use('*', (req, res) => res.status(404).send('Files - Not Found - Error'));
router.use('*', (req, res) => res.status(404).redirect('/files/'));

export default router;
