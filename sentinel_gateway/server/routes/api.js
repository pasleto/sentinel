import express from 'express';
import * as constant from '../utils/constant.js';
import * as morgan from '../controllers/morgan.controller.js';
import * as middleware from '../controllers/middleware.controller.js';

const router = express.Router();
/*
Backend /api/* routes
*/
router.use(morgan.apiLoggerHandler);

// public routes
router.get('/', (req, res) => res.status(200).send(constant.api_root_message));

router.use(middleware.notFoundHandler);

export default router;
