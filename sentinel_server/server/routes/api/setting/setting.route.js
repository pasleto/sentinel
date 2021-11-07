const express = require('express');
const router = express.Router();
const controller = require('./setting.controller');

router.get('/', controller.getAll);
router.get('/:id', controller.getOneById);
router.patch('/:id', controller.updateOneById);
router.delete('/:id', controller.deleteOneById);

module.exports = router;
