const service = require('./setting.service');

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

module.exports = {
  getAll,
  getOneById,
  updateOneById,
  deleteOneById,
};