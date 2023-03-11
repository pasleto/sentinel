import rfs from 'rotating-file-stream';
import { resolve } from 'path';
import morgan from 'morgan';
import moment from 'moment-timezone';

const timezone = Intl.DateTimeFormat().resolvedOptions().timeZone;
const pad = num => (num > 9 ? '' : '0') + num;
const timestring = (tz) => { return moment().tz(tz).format('DD-MM-YYYY HH:mm:ss'); };
morgan.token('timestamp', (req, res, tz) => { return timestring(tz); });

const generator = (time, index) => {
  if (!time) return 'file.log';
  var year = time.getFullYear();
  var month = pad(time.getMonth() + 1);
  var day = pad(time.getDate());
  return `${year}/${month}/${day}-(${index}).log`; // 2023/02/08-(1).log
  // return `${year}_${month}_${day}-(${index}).log`; // 2023_02_08-(1).log
};
  
morgan.token('apiUser', (req, res) => { // ?
  var authheader = req.headers.authorization;
  if (authheader) {
    var credentials = new Buffer.from(authheader.split(' ')[1],'base64').toString().split(':');
    return credentials[0];
  } else {
    if (req.body && req.body.username) {
      return req.body.username;
    } else {
      return `unauthenticated`;
    }
  }
});

var apiLogStream = rfs.createStream(generator, { interval: '1d', path: resolve('logs/api') });
morgan.format('api', `[:timestamp[${timezone}]] :remote-addr [:apiUser] :method [:status] :url :res[content-length] (:response-time ms)`);
const apiLoggerHandler = morgan('api', { stream: apiLogStream });

var filesLogStream = rfs.createStream(generator, { interval: '1d', path: resolve('logs/files') });
morgan.format('files', `[:timestamp[${timezone}]] :remote-addr :method [:status] :url :res[content-length] (:response-time ms)`);
const filesLoggerHandler = morgan('files', { stream: filesLogStream });

var clientLogStream = rfs.createStream(generator, { interval: '1d', path: resolve('logs/client') });
morgan.format('client', `[:timestamp[${timezone}]] :remote-addr :method [:status] :url :res[content-length] (:response-time ms)`);
const clientLoggerHandler = morgan('client', { stream: clientLogStream });

var ntpLogStream = rfs.createStream(generator, { interval: '1d', path: resolve('logs/ntp') });
const ntpLoggerHandler = (family, address, status, message) => {
  ntpLogStream.write(`[${timestring(timezone)}] ${address} ${family} ${status} ${message} \n`);
};

export {
  apiLoggerHandler, // handle api access
  filesLoggerHandler, // handle files access
  clientLoggerHandler, // handle client access
  ntpLoggerHandler, // handle ntp log
};
