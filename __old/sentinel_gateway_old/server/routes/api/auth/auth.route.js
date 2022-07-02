const express = require('express');
const router = express.Router();
const controller = require('./auth.controller');

router.post('/local', controller.authenticateLocal);
router.post('/ldap', controller.authenticateLdap);
router.post('/windows', controller.authenticateWindows);

// ? refresh endpoint

module.exports = router;
