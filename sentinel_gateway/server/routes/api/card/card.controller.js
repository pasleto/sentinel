const service = require('./card.service');

function getAll(req, res, next) {
  service.getAll()
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getAllFull(req, res, next) {
  service.getAllFull()
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getAllUnassigned(req, res, next) {
  service.getAllUnassigned()
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getAllActive(req, res, next) {
  service.getAllActive()
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getAllUnactive(req, res, next) {
  service.getAllUnactive()
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getOneById(req, res, next) {
  service.getOneById(req.params.id)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getOneByIdFull(req, res, next) {
  service.getOneByIdFull(req.params.id)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function getOneByHexUid(req, res, next) {
  service.getOneByHexUid(req.params.uid)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function updateOneById(req, res, next) {
  service.updateOneById(req.params.id, req.body)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function returnOneById(req, res, next) {
  service.returnOneById(req.params.id)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function deactivateOneById(req, res, next) {
  service.deactivateOneById(req.params.id)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function create(req, res, next) {
  service.create(req.body)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

module.exports = {
  getAll,
  getAllFull,
  getAllUnassigned,
  getAllActive,
  getAllUnactive,
  getOneById,
  getOneByIdFull,
  getOneByHexUid,
  updateOneById,
  returnOneById,
  deactivateOneById,
  create,
};