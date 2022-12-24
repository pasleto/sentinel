import error from './error.controller.js';

function notFoundHandler(req, res, next) {
  const err = new error.EndpointNotFoundError(req.originalUrl);
  next(err);
};


function errorHandler(err, req, res, next) { // ? for rest api
  if(err instanceof error.EndpointNotFoundError) {
    return res.status(err.statusCode).json({ name: err.name, statusCode: err.statusCode, endpoint: err.endpoint, message: err.message });
  }
  // if(err instanceof error.ValidationError) {
  //   return res.status(err.statusCode).json({ name: err.name, statusCode: err.statusCode, message: err.message });
  // }
  // if(err instanceof error.ConflictError) {
  //   return res.status(err.statusCode).json({ name: err.name, statusCode: err.statusCode, message: err.message });
  // }
  // if(err instanceof error.PropertyRequiredError) {
  //   return res.status(err.statusCode).json({ name: err.name, statusCode: err.statusCode, property: err.property, message: err.message });
  // }
  // if(err instanceof error.NotFoundError) {
  //   return res.status(err.statusCode).json({ name: err.name, statusCode: err.statusCode, item: err.item, message: err.message });
  // }
  return res.status(500).json({ name: err.name, statusCode: 500, message: err.message });
}

export {
  notFoundHandler,
  errorHandler
};