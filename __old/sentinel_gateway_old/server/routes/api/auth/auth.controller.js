const service = require('./auth.service');

function authenticateLocal(req, res, next) {
  service.authenticateLocal(req.body)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function authenticateLdap(req, res, next) {
  service.authenticateLdap(req.body)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

function authenticateWindows(req, res, next) {
  service.authenticateWindows(req.body)
    .then(result => res.status(200).send(result))
    .catch(err => next(err));
}

module.exports = {
  authenticateLocal,
  authenticateLdap,
  authenticateWindows
};