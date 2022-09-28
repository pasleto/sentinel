
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

// TODO - remove this file

class GenericError extends Error {
  constructor(message) {
    super(message);
    this.name = this.constructor.name;
    this.statusHttp = 500;
    this.statusCode = 'NOK';
    Error.captureStackTrace(this, ConflictError)
  }
}

class ConflictError extends Error {
  constructor(message) {
    super(message); // super('Conflict: ' + message);
    this.name = this.constructor.name;
    this.statusHttp = 409;
    this.statusCode = 'NOK';
    Error.captureStackTrace(this, ConflictError)
  }
}

class ValidationError extends Error {
  constructor(message) {
    super(message);
    this.name = this.constructor.name;
    this.statusHttp = 400;
    this.statusCode = 'NOK';
    Error.captureStackTrace(this, ValidationError);
  }
}

class PropertyRequiredError extends Error {
  constructor(message) {
    super(message); //super('Missing property: ' + property);
    this.name = this.constructor.name;
    this.statusHttp = 400;
    this.statusCode = 'NOK';
    Error.captureStackTrace(this, PropertyRequiredError)
  }
}

class NotFoundError extends Error {
  constructor(message) {
    super(message); // super('Not found: ' + item);
    this.name = this.constructor.name;
    this.statusHttp = 404;
    this.statusCode = 'NOK';
    Error.captureStackTrace(this, NotFoundError)
  }
}

// class EndpointNotFoundError extends Error {
//   constructor(endpoint) {
//     super('Endpoint not found: ' + endpoint);
//     this.name = this.constructor.name;
//     this.statusCode = 404;
//     this.endpoint = endpoint;
//     Error.captureStackTrace(this, EndpointNotFoundError)
//   }
// }



export default {
  // DatabaseConnectionError,
  ConflictError,
  ValidationError,
  PropertyRequiredError,
  NotFoundError,
  // EndpointNotFoundError
};