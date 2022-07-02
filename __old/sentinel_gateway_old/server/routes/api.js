const express = require('express');
const router = express.Router();
const constant = require('../helpers/constant');
const morgan = require('../helpers/morgan');
const middleware = require('../helpers/middleware');
/*
Backend /api/* routes
*/
router.use(morgan.apiLoggerHandler);

// public routes
router.get('/', (req, res) => res.status(200).send(constant.api_message));
router.use('/auth', require('./api/auth/auth.route'));

router.use('/setting', require('./api/setting/setting.route'));
router.use('/employee', require('./api/employee/employee.route'));
router.use('/department', require('./api/department/department.route'));
router.use('/unit', require('./api/unit/unit.route'));
router.use('/card', require('./api/card/card.route'));

// protected routes
// router.use('/user', require('./api/user/user.route'));
// router.use('/employee', require('./api/employee/employee.route'));

router.use(middleware.notFoundHandler);
router.use(middleware.errorHandler);

module.exports = router;
