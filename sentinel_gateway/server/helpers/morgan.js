const rfs = require('rotating-file-stream');
const path = require('path');
const morgan = require('morgan');
const moment = require('moment-timezone');


const pad = num => (num > 9 ? '' : '0') + num;
morgan.token('timestamp', (req, res, tz) => { return moment().tz(tz).format('DD-MM-YYYY HH:mm:ss'); });

// morgan.token('authUser', (req, res) => {
//   if (req.jwtUser && req.jwtUser.user && req.jwtUser.user.username) return req.jwtUser.user.username;
//   else return `unauthenticated`;
// });
// morgan.format('myformat', ':remote-addr [:authUser] [:timestamp[Europe/Prague]] :method [:status] :url :res[content-length] (:response-time ms)');

const apiGenerator = (time, index) => {
  if (!time) return 'api.log';
  var year = time.getFullYear();
  var month = pad(time.getMonth() + 1);
  var day = pad(time.getDate());
  return `${year}/${month}/${day}-(${index})-api.log`;
};

var apiLogStream = rfs.createStream(apiGenerator(), {
  interval: '1d',
  path: path.join(__dirname, '../../logs/api')
});

// morgan.format('api', ':remote-addr [:timestamp[Europe/Prague]] :method [:status] :url :res[content-length] (:response-time ms)');
morgan.format('api', `:remote-addr [:timestamp[${process.env.APP_TIMEZONE}]] :method [:status] :url :res[content-length] (:response-time ms)`);
const apiLoggerHandler = morgan('api', { stream: apiLogStream });


const databaseGenerator = (time, index) => {
  if (!time) return 'database.log';
  var year = time.getFullYear();
  var month = pad(time.getMonth() + 1);
  var day = pad(time.getDate());
  return `${year}/${month}/${day}-(${index})-database.log`;
};

var databaseLogStream = rfs.createStream(databaseGenerator(), {
  interval: '1d',
  path: path.join(__dirname, '../../logs/database')
});

morgan.format('database', `:remote-addr [:timestamp[${process.env.APP_TIMEZONE}]] :method [:status] :url :res[content-length] (:response-time ms)`);
const databaseLoggerHandler = morgan('database', { stream: databaseLogStream });


const serverGenerator = (time, index) => { // TODO - replace this from morgan to maybe winston? to make logs from server console into file
  if (!time) return 'server.log';
  var year = time.getFullYear();
  var month = pad(time.getMonth() + 1);
  var day = pad(time.getDate());
  return `${year}/${month}/${day}-(${index})-server.log`;
};

var serverLogStream = rfs.createStream(serverGenerator(), {
  interval: '1d',
  path: path.join(__dirname, '../../logs/server')
});

morgan.format('server', `:remote-addr [:timestamp[${process.env.APP_TIMEZONE}]] :method [:status] :url :res[content-length] (:response-time ms)`);
const serverLoggerHandler = morgan('server', { stream: serverLogStream });


const filesGenerator = (time, index) => {
  if (!time) return 'files.log';
  var year = time.getFullYear();
  var month = pad(time.getMonth() + 1);
  var day = pad(time.getDate());
  return `${year}/${month}/${day}-(${index})-files.log`;
};

var filesLogStream = rfs.createStream(filesGenerator(), {
  interval: '1d',
  path: path.join(__dirname, '../../logs/files')
});

morgan.format('files', `:remote-addr [:timestamp[${process.env.APP_TIMEZONE}]] :method [:status] :url :res[content-length] (:response-time ms)`);
const filesLoggerHandler = morgan('files', { stream: filesLogStream });


const clientGenerator = (time, index) => {
  if (!time) return 'client.log';
  var year = time.getFullYear();
  var month = pad(time.getMonth() + 1);
  var day = pad(time.getDate());
  return `${year}/${month}/${day}-(${index})-client.log`;
};

var clientLogStream = rfs.createStream(clientGenerator(), {
  interval: '1d',
  path: path.join(__dirname, '../../logs/client')
});

morgan.format('client', `:remote-addr [:timestamp[${process.env.APP_TIMEZONE}]] :method [:status] :url :res[content-length] (:response-time ms)`);
const clientLoggerHandler = morgan('client', { stream: clientLogStream });

module.exports = {
  apiLoggerHandler, // handle api access
  databaseLoggerHandler, // handle database access
  serverLoggerHandler, // handle server access
  filesLoggerHandler, // handle files access
  clientLoggerHandler, // handle client access
};
