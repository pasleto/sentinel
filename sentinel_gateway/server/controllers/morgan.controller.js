import rfs from 'rotating-file-stream';
import { resolve } from 'path';
import morgan from 'morgan';
import moment from 'moment-timezone';

const timezone = Intl.DateTimeFormat().resolvedOptions().timeZone;
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
  // return `${year}/${month}/${day}-(${index})-api.log`;
  return `${year}_${month}_${day}-(${index})-api.log`;
};

var apiLogStream = rfs.createStream(apiGenerator, {
  interval: '1d',
  path: resolve('logs/api')
});

morgan.format('api', `:remote-addr [:timestamp[${timezone}]] :method [:status] :url :res[content-length] (:response-time ms)`);
const apiLoggerHandler = morgan('api', { stream: apiLogStream });

const filesGenerator = (time, index) => {
  if (!time) return 'files.log';
  var year = time.getFullYear();
  var month = pad(time.getMonth() + 1);
  var day = pad(time.getDate());
  // return `${year}/${month}/${day}-(${index})-files.log`;
  return `${year}_${month}_${day}-(${index})-files.log`;
};

var filesLogStream = rfs.createStream(filesGenerator, {
  interval: '1d',
  path: resolve('logs/files')
});

morgan.format('files', `:remote-addr [:timestamp[${timezone}]] :method [:status] :url :res[content-length] (:response-time ms)`);
const filesLoggerHandler = morgan('files', { stream: filesLogStream });

const clientGenerator = (time, index) => {
  if (!time) return 'client.log';
  var year = time.getFullYear();
  var month = pad(time.getMonth() + 1);
  var day = pad(time.getDate());
  // return `${year}/${month}/${day}-(${index})-client.log`;
  return `${year}_${month}_${day}-(${index})-client.log`;
};

var clientLogStream = rfs.createStream(clientGenerator, {
  interval: '1d',
  path: resolve('logs/client')
});

morgan.format('client', `:remote-addr [:timestamp[${timezone}]] :method [:status] :url :res[content-length] (:response-time ms)`);
const clientLoggerHandler = morgan('client', { stream: clientLogStream });

export {
  apiLoggerHandler, // handle api access
  filesLoggerHandler, // handle files access
  clientLoggerHandler, // handle client access
};
