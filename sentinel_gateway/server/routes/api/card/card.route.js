const express = require('express');
const router = express.Router();
const controller = require('./card.controller');

router.get('/', controller.getAll); // Done
router.get('/full', controller.getAllFull); // Done
router.get('/active', controller.getAllActive); // Done
router.get('/unactive', controller.getAllUnactive); // Done
router.get('/unassigned', controller.getAllUnassigned); // Done
router.get('/hex/:uid', controller.getOneByHexUid); // Done
router.get('/full/:id', controller.getOneByIdFull); // Done
router.get('/:id', controller.getOneById); // Done
router.post('/', controller.create); // Done
router.patch('/:id', controller.updateOneById);
router.delete('/return/:id', controller.returnOneById);
router.delete('/deactivate/:id', controller.deactivateOneById);

module.exports = router;
