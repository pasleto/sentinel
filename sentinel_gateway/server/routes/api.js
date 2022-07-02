import express from 'express';
import * as constant from '../utils/constant.js';
import * as morgan from '../utils/morgan.js';
import * as middleware from '../utils/middleware.js';

const router = express.Router();
/*
Backend /api/* routes
*/
router.use(morgan.apiLoggerHandler);

// public routes
router.get('/', (req, res) => res.status(200).send(constant.api_root_message));

// eg.
// router.use('/auth', require('./api/auth/auth.route'));
// router.use('/setting', require('./api/setting/setting.route'));
// router.use('/employee', require('./api/employee/employee.route'));
// router.use('/department', require('./api/department/department.route'));
// router.use('/unit', require('./api/unit/unit.route'));
// router.use('/card', require('./api/card/card.route'));

// protected routes
// eg.
// router.use('/user', require('./api/user/user.route'));
// router.use('/employee', require('./api/employee/employee.route'));

router.use(middleware.notFoundHandler);
router.use(middleware.errorHandler);

export default router;
