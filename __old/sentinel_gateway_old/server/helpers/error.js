
// class ApplicationError extends Error {
  
// }

// class DatabaseConnectionError extends Error {
//   constructor() {
//     super('Connection to database failed!');
//     this.name = this.constructor.name;
//     this.statusCode = 500;
//     Error.captureStackTrace(this, DatabaseConnectionError);
//   }
// }

class ConflictError extends Error {
  constructor(message) {
    super('Conflict: ' + message);
    this.name = this.constructor.name;
    this.statusCode = 409;
    Error.captureStackTrace(this, ConflictError)
  }
}

class ValidationError extends Error {
  constructor(message) {
    super(message);
    this.name = this.constructor.name;
    this.statusCode = 400;
    Error.captureStackTrace(this, ValidationError);
  }
}

class PropertyRequiredError extends Error {
  constructor(property) {
    super('Missing property: ' + property);
    this.name = this.constructor.name;
    this.statusCode = 400;
    this.property = property;
    Error.captureStackTrace(this, PropertyRequiredError)
  }
}

class NotFoundError extends Error {
  constructor(item) {
    super('Not found: ' + item);
    this.name = this.constructor.name;
    this.statusCode = 404;
    this.item = item;
    Error.captureStackTrace(this, NotFoundError)
  }
}

class EndpointNotFoundError extends Error {
  constructor(endpoint) {
    super('Endpoint not found: ' + endpoint);
    this.name = this.constructor.name;
    this.statusCode = 404;
    this.endpoint = endpoint;
    Error.captureStackTrace(this, EndpointNotFoundError)
  }
}



module.exports = {
  // DatabaseConnectionError,
  ConflictError,
  ValidationError,
  PropertyRequiredError,
  NotFoundError,
  EndpointNotFoundError
};