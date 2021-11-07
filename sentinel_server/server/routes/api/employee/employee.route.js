const express = require('express');
const router = express.Router();
const controller = require('./employee.controller');

router.get('/', controller.getAll);
router.get('/:id', controller.getOneById);
router.post('/', controller.create);
router.patch('/:id', controller.updateOneById);
router.delete('/', controller.deleteAll);
router.delete('/:id', controller.deleteOneById);

module.exports = router;
