import logSymbols from 'log-symbols';
import getmac from 'getmac';
import { SimpleCrypto } from 'simple-crypto-js';
import bcrypt from 'bcryptjs';
import { exec } from 'child_process';
import crypto from 'crypto';
import nodeMachineId from 'node-machine-id';
import forge from 'node-forge';
import { settingService } from '../controllers/mongo.controller.js';
import fs from 'fs';
import tls from 'tls';
import https from 'https';

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

function execPromise(command) {
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

async function setServerProxy(setProxy, proxy_string) {
  if (setProxy) {
    // await execPromise(`export {http,https,ftp}_proxy==${proxy_string}`); // ?
    await execPromise(`export http_proxy=${proxy_string}`);
    await execPromise(`export https_proxy=${proxy_string}`);
    await execPromise(`export ftp_proxy=${proxy_string}`);
    console.log(logSymbols.info, '[PROXY] Set - ' + proxy_string);
  } else {
    // await execPromise('unset {http,https,ftp}_proxy'); // ?
    await execPromise('unset http_proxy');
    await execPromise('unset https_proxy');
    await execPromise('unset ftp_proxy');
    console.log(logSymbols.info, '[PROXY] Unset');
  }
}

async function initServerProxy() {
  try {
    var proxy_string = await settingService.getOne({ scope: 'proxy', name: 'url_string' });
    if (proxy_string.value) {
      await setServerProxy(true, proxy_string.value);
    } else {
      await setServerProxy(false, undefined);
    }
  } catch (error) {
    throw new Error('Unable to initialize server proxy!'); // TODO - maybe just console the error
  }
};

function getServerTimezone() {
  return Intl.DateTimeFormat().resolvedOptions().timeZone;
}

async function setServerTimezone(newTz) { // TODO - maybe just use UTC
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

// async function getServerInternalIp() {
//   return await execPromise('hostname -i');
// }

// async function getServerIps() {
//   return await execPromise('hostname -I');
// }

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

function loadCertificates() {
  const list = [
    '/etc/ssl/certs/ca-certificates.crt',
    '/etc/pki/tls/certs/ca-bundle.crt',
    '/etc/ssl/ca-bundle.pem',
    '/etc/pki/tls/cacert.pem',
    '/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem',
  ];

  // var additionalCerts = [];
  var finalCerts = [
    ...tls.rootCertificates,
  ];

  for (let certPath of list) {
    try {
      const certFile = fs.readFileSync(certPath, 'utf-8');
      // additionalCerts.push(...certFile.split(/(?=-----BEGIN\sCERTIFICATE-----)/g).map((cert) => cert));
      if (!finalCerts.includes(...certFile.split(/(?=-----BEGIN\sCERTIFICATE-----)/g).map((cert) => cert))) {
        finalCerts.push(...certFile.split(/(?=-----BEGIN\sCERTIFICATE-----)/g).map((cert) => cert));
      }
    } catch (error) {
      if (error.code !== 'ENOENT') {
        throw error;
      }
    }
  }

  // https.globalAgent.options.ca = [
  //   ...tls.rootCertificates,
  //   ...additionalCerts
  // ];
  https.globalAgent.options.ca = [
    ...finalCerts,
  ];

  console.log(logSymbols.info, '[CERTIFICATES]', 'System certificates loaded');
}

function _toPositiveHex (hexString) {
  var mostSiginficativeHexAsInt = parseInt(hexString[0], 16);
  if (mostSiginficativeHexAsInt < 8) {
    return hexString;
  }
  mostSiginficativeHexAsInt -= 8;
  return mostSiginficativeHexAsInt.toString() + hexString.substring(1);
}

function _generateRandomCertificateSerialNumber() {
  return _toPositiveHex(forge.util.bytesToHex(forge.random.getBytesSync(16)));
}

function generateRootCertificate(common_name) { // ROOT
  var root_keys = forge.pki.rsa.generateKeyPair(4096);
  var root_cert = forge.pki.createCertificate();
  root_cert.publicKey = root_keys.publicKey;
  root_cert.serialNumber = _generateRandomCertificateSerialNumber();
  root_cert.validity.notBefore = new Date(); // now
  root_cert.validity.notAfter = new Date(253402257600000); // 9999-12-31T12:00:00.000Z -> Friday 31. December 9999 12:00:00
  root_cert.setSubject([{
    name: 'commonName',
    value: `${common_name}`
  }]);
  root_cert.setIssuer([{
    name: 'commonName',
    value: `${common_name}`
  }]);
  root_cert.setExtensions([{
    name: 'basicConstraints',
    cA: true,
  }, {
    name: 'keyUsage',
    keyCertSign: true,
    digitalSignature: true,
    nonRepudiation: true,
    keyEncipherment: true,
    dataEncipherment: true
  }, {
    name: 'extKeyUsage',
    serverAuth: true,
    clientAuth: true,
    codeSigning: true,
    emailProtection: true,
    timeStamping: true
  }, {
    name: 'nsCertType',
    client: true,
    server: true,
    email: true,
    objsign: true,
    sslCA: true,
    emailCA: true,
    objCA: true
  }, {
    name: 'subjectKeyIdentifier'
  }]);
  root_cert.sign(root_keys.privateKey, forge.md.sha256.create());
  var root_pem = {
    privateKey: forge.pki.privateKeyToPem(root_keys.privateKey),
    publicKey: forge.pki.publicKeyToPem(root_keys.publicKey),
    certificate: forge.pki.certificateToPem(root_cert)
  };
  return root_pem;
}

async function getRootCertificate() {
  try {
    const root_cert = await settingService.getOne({ scope: 'general', name: 'root_certificate' });
    return root_cert.value;
  } catch (error) {
    throw new Error('Unable to get root certificate files!');
  }
}

async function _getRootPrivateKey() {
  try {
    const root_key = await settingService.getOne({ scope: 'general', name: 'root_private_key' });
    return root_key.value;
  } catch (error) {
    throw new Error('Unable to get root certificate private key!');
  }
}

async function _generateServerCertificate(root_certificate_pem, root_private_key_pem) { // SERVER // ?
  var server_keys = forge.pki.rsa.generateKeyPair(4096);
  var server_cert = forge.pki.createCertificate();
  server_cert.publicKey = server_keys.publicKey;
  server_cert.serialNumber = _generateRandomCertificateSerialNumber();
  server_cert.validity.notBefore = new Date(); // now
  server_cert.validity.notAfter = new Date(253402257600000); // 9999-12-31T12:00:00.000Z -> Friday 31. December 9999 12:00:00 // ?
  server_cert.setSubject([{
    name: 'commonName',
    value: await getServerDomainHostname()
  }]);
  server_cert.setIssuer(forge.pki.certificateFromPem(root_certificate_pem).subject.attributes);
  server_cert.setExtensions([{
    name: 'subjectAltName',
    altNames: [{
      type: 2, // DNS
      value: await getServerDomainHostname()
    },{
      type: 2, // DNS
      value: await getServerHostname()
    }]
  }, {
    name: 'subjectKeyIdentifier'
  },
  // { // TODO
  //   name: 'authorityKeyIdentifier',
  //   // keyIdentifier: forge.pki.certificateFromPem(root_certificate_pem).getExtension('subjectKeyIdentifier').subjectKeyIdentifier,
  //   // keyIdentifier: forge.pki.certificateFromPem(root_certificate_pem).getExtension('subjectKeyIdentifier').value,
  //   // keyIdentifier: forge.pki.certificateFromPem(root_certificate_pem).getExtension('subjectKeyIdentifier').value,
  //   // value: forge.pki.certificateFromPem(root_certificate_pem).getExtension('subjectKeyIdentifier').value.value
  // }
  ]);
  server_cert.sign(forge.pki.privateKeyFromPem(root_private_key_pem), forge.md.sha256.create());
  var server_pem = {
    privateKey: forge.pki.privateKeyToPem(server_keys.privateKey),
    publicKey: forge.pki.publicKeyToPem(server_keys.publicKey),
    certificate: forge.pki.certificateToPem(server_cert)
  };
  return server_pem;
}

async function generateServerCertificate() {
  try {
    const root_cert_pem = await getRootCertificate();
    const root_key_pem = await _getRootPrivateKey();
    return await _generateServerCertificate(root_cert_pem, root_key_pem);
  } catch (error) {
    throw new Error('Unable to generate server certificate!');
  }
}

function _generateClientCertificate(common_name, root_certificate_pem, root_private_key_pem) { // CLIENT
  var client_keys = forge.pki.rsa.generateKeyPair(4096);
  var client_cert = forge.pki.createCertificate();
  client_cert.publicKey = client_keys.publicKey;
  client_cert.serialNumber = _generateRandomCertificateSerialNumber();
  client_cert.validity.notBefore = new Date(); // now
  client_cert.validity.notAfter = new Date(253402257600000); // 9999-12-31T12:00:00.000Z -> Friday 31. December 9999 12:00:00
  client_cert.setSubject([{
    name: 'commonName',
    value: `${common_name}`
  }]);
  client_cert.setIssuer(forge.pki.certificateFromPem(root_certificate_pem).subject.attributes);
  client_cert.setExtensions([{
    name: 'subjectKeyIdentifier'
  },
  // { // TODO
  //   name: 'authorityKeyIdentifier',
  //   keyIdentifier: forge.pki.certificateFromPem(root_certificate_pem).getExtension('subjectKeyIdentifier').subjectKeyIdentifier
  // }
  ]);
  client_cert.sign(forge.pki.privateKeyFromPem(root_private_key_pem), forge.md.sha256.create());
  var client_pem = {
    privateKey: forge.pki.privateKeyToPem(client_keys.privateKey),
    publicKey: forge.pki.publicKeyToPem(client_keys.publicKey),
    certificate: forge.pki.certificateToPem(client_cert)
  };
  return client_pem;
}

async function generateClientCertificate(client_name) {
  try {
    const root_cert_pem = await getRootCertificate();
    const root_key_pem = await _getRootPrivateKey();
    return await _generateClientCertificate(client_name, root_cert_pem, root_key_pem);
  } catch (error) {
    throw new Error('Unable to generate client certificate!');
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
  initServerProxy,
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
  decryptCard,
  loadCertificates,
  generateRootCertificate,
  getRootCertificate,
  generateServerCertificate,
  generateClientCertificate
};
