const service = require('./unit.service');

function getAll(req, res, next) {
  service.getAll()
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getOneById(req, res, next) {
  service.getOneById(req.params.id)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function updateOneById(req, res, next) {
  service.updateOneById(req.params.id, req.body)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function deleteOneById(req, res, next) {
  service.deleteOneById(req.params.id)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function deleteAll(req, res, next) {
  service.deleteAll()
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function create(req, res, next) {
  service.create(req.body)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getAllTypes(req, res, next) {
  service.getAllTypes()
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getAllTypesFull(req, res, next) {
  service.getAllTypesFull()
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getOneTypeById(req, res, next) {
  service.getOneTypeById(req.params.id)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getOneTypeByIdFull(req, res, next) {
  service.getOneTypeByIdFull(req.params.id)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function updateOneTypeById(req, res, next) {
  service.updateOneTypeById(req.params.id, req.body)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function deleteOneTypeById(req, res, next) {
  service.deleteOneTypeById(req.params.id)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function deleteAllTypes(req, res, next) {
  service.deleteAllTypes()
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function createType(req, res, next) {
  service.createType(req.body)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

module.exports = {
  getAll,
  getOneById,
  updateOneById,
  deleteOneById,
  deleteAll,
  create,
  getAllTypes,
  getAllTypesFull,
  getOneTypeById,
  getOneTypeByIdFull,
  updateOneTypeById,
  deleteOneTypeById,
  deleteAllTypes,
  createType,
};