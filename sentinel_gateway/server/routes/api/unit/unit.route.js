const express = require('express');
const router = express.Router();
const controller = require('./unit.controller');

router.get('/type/', controller.getAllTypes); // Done
router.get('/type/full', controller.getAllTypesFull); // Done
router.get('/type/:id', controller.getOneTypeById); // Done
router.get('/type/full/:id', controller.getOneTypeByIdFull); // Done
router.post('/type/', controller.createType); // Done
router.patch('/type/:id', controller.updateOneTypeById);
router.delete('/type/', controller.deleteAllTypes);
router.delete('/type/:id', controller.deleteOneTypeById);

router.get('/', controller.getAll);
router.get('/:id', controller.getOneById);
router.post('/', controller.create);
router.patch('/:id', controller.updateOneById);
router.delete('/', controller.deleteAll);
router.delete('/:id', controller.deleteOneById);

module.exports = router;
