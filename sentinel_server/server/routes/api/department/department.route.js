const express = require('express');
const router = express.Router();
const controller = require('./department.controller');

router.get('/', controller.getAll); // get array of all departments

// router.get('/protected', controller.getAllProtected); // get array of all protected departments
// router.get('/root', controller.getAllRoot); // get array of all root departments

// router.get('/tree', controller.getTree); // get tree of all departments including departments and employees

router.get('/:id', controller.getOneById); // get one department by ID
router.post('/', controller.create); // create new department
router.patch('/:id', controller.updateOneById); // update department by ID
router.delete('/', controller.deleteAll); // delete all departments [not protected && not root]
router.delete('/:id', controller.deleteOneById); // delete department by ID [not protected && not root]

// on department delete - handle items contained inside

// router
//   .route('/')
//   .get((req,res,next) => {

//   })
//   .post((req,res,next) => {
    
//   })
//   .delete((req,res,next) => {
    
//   });

module.exports = router;
