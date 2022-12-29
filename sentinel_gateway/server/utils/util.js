import logSymbols from 'log-symbols';
import getmac from 'getmac';
import { SimpleCrypto } from 'simple-crypto-js';
import bcrypt from 'bcryptjs';
import { exec } from 'child_process';
import crypto from 'crypto';
import nodeMachineId from 'node-machine-id';
import { settingService } from '../controllers/mongo.controller.js';

function crypt(key, value) {
  const simpleCrypto = new SimpleCrypto(key);
  return simpleCrypto.encrypt(value);
}

function decrypt(key, value) {
  const simpleCrypto = new SimpleCrypto(key);
  return simpleCrypto.decrypt(value);
}

function generatePasswordHash(plain_password) {
  return bcrypt.hashSync(plain_password.trim(), 10);
}

function comparePasswordHash(plain_password, hashed_password) { // ? - make custom error if password does not match
  return bcrypt.compareSync(plain_password, hashed_password);
}

function execPromise(command) { // TODO - try catch on usage
  return new Promise(function(resolve, reject) {
    exec(command, (error, stdout, stderr) => {
      if (error) {
        reject(error);
        return;
      }
      resolve(stdout.trim());
    });
  });
}

// const stripAccents = (function () {
//   var inChrs = 'àáâãäçèéêëěěìíîïñòóôõöùúûüýÿÀÁÂÃÄÇÈÉÊËÌÍÎÏÑÒÓÔÕÖÙÚÛÜÝřŘšŠťŤďĎňŇĚÉžŽčČ';
//   var outChrs = 'aaaaaceeeeeeiiiinooooouuuuyyAAAAACEEEEIIIINOOOOOUUUUYrRsStTdDnNEEzZcC';
//   var charsRgx = new RegExp('[' + inChrs + ']', 'g');
//   var transl = {};
//   var i;
//   var lookup = function (m) { return transl[m] || m; };

//   for (i = 0; i < inChrs.length; i++) {
//     transl[ inChrs[i] ] = outChrs[i];
//   }

//   return function (string) { return string.replace(charsRgx, lookup); };
// })();

function stripAccents(text) {
  // var inChrs = 'àáâãäçèéêëěěìíîïñòóôõöùúûüýÿÀÁÂÃÄÇÈÉÊËÌÍÎÏÑÒÓÔÕÖÙÚÛÜÝřŘšŠťŤďĎňŇĚÉžŽčČ';
  // var outChrs = 'aaaaaceeeeeeiiiinooooouuuuyyAAAAACEEEEIIIINOOOOOUUUUYrRsStTdDnNEEzZcC';
  var inChrs = 'ÀÁÂÃÄÅàáâãäåÒÓÔÕÕÖØòóôõöøÈÉÊËĚèéêëěðČÇçčÐĎďÌÍÎÏìíîïĽľÙÚÛÜŮùúûüůŇÑñňŘřŠšŤťŸÝÿýŽž';
  var outChrs = 'AAAAAAaaaaaaOOOOOOOooooooEEEEEeeeeeeCCccDDdIIIIiiiiLlUUUUUuuuuuNNnnRrSsTtYYyyZz';
  var charsRgx = new RegExp('[' + inChrs + ']', 'g');
  var transl = {};
  var i;
  var lookup = function (m) { return transl[m] || m; };

  for (i = 0; i < inChrs.length; i++) {
    transl[ inChrs[i] ] = outChrs[i];
  }

  return text.replace(charsRgx, lookup);
};

function textToNag(text) {
  return stripAccents(text).toLowerCase().replace(/[^a-z0-9\s]/gi, '').replace(/\s+/g, '_');
}

async function setServerProxy(setProxy, proxy_string) { // TODO - check and run on server startup aswell - after mongo connect (in init function)
  if (setProxy) {
    // await execPromise(`export {http,https,ftp}_proxy==${proxy_string.value}`); // TODO - on linux environment
    console.log(logSymbols.info, '[Proxy] Set - ' + proxy_string);
  } else {
    // await execPromise('unset {http,https,ftp}_proxy'); // TODO - on linux environment
    console.log(logSymbols.info, '[Proxy] Unset');
  }
}

function getServerTimezone() {
  return Intl.DateTimeFormat().resolvedOptions().timeZone;
}

async function setServerTimezone(newTz) { // TODO
  // await execPromise(`timedatectl set-timezone ${newTz}`); // timedatectl set-timezone newTz // TODO - on linux environment
  global.serverRebootRequired  = true;
  global.io.of('client-app').to('admin').emit('server-reboot-required', true); // to all clients in room "admin" in namespace "client-app"

  console.log('New Server Timezone: ' + newTz);
  console.log('Server Reboot Required: ' + global.serverRebootRequired);
}

function getServerMacAddress() {
  return getmac().toUpperCase();
}

function getServerMachineId() {
  return nodeMachineId.machineIdSync();
}

async function getServerHostname() {
  return await execPromise('hostname -s');
}

async function getServerDomain() {
  return await execPromise('hostname -d');
}

async function getServerDomainHostname() {
  return await execPromise('hostname -f');
}

async function getServerUpdatesCount() {
  // apt-get -q -y --ignore-hold --allow-change-held-packages --allow-unauthenticated -s dist-upgrade | /bin/grep  ^Inst | wc -l
  // apt-get upgrade -s |grep -P '^\d+ upgraded'|cut -d" " -f1
  // apt-get dist-upgrade -s --quiet=2 | grep ^Inst | wc -l
  return await execPromise('apt-get dist-upgrade -s --quiet=2 | grep ^Inst | wc -l');
}

function generateRandomHash() {
  return crypto.randomBytes(20).toString('hex');
}

async function cryptCard(card) {
  try {
    var hash_card = await settingService.getOne({ scope: 'general', name: 'hash_card' });
    return crypt(hash_card.value, card);
  } catch (error) {
    throw new Error('Unable to crypt this card!');
  }
}

async function decryptCard(card) {
  try {
    var hash_card = await settingService.getOne({ scope: 'general', name: 'hash_card' });
    return decrypt(hash_card.value, card);
  } catch (error) {
    console.log(error.message); // Invalid encrypted text received. Decryption halted.
    throw new Error('Invalid card!');
  }
}



export default {
  generatePasswordHash,
  comparePasswordHash,
  execPromise,
  crypt,
  decrypt,
  stripAccents,
  textToNag,
  setServerProxy,
  getServerTimezone,
  setServerTimezone,
  getServerMacAddress,
  getServerMachineId,
  getServerHostname,
  getServerDomain,
  getServerDomainHostname,
  getServerUpdatesCount,
  generateRandomHash,
  cryptCard,
  decryptCard
};
