import getmac from 'getmac';
import { exec } from 'child_process';

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

const stripAccents = (function () {
  var inChrs = 'àáâãäçèéêëěěìíîïñòóôõöùúûüýÿÀÁÂÃÄÇÈÉÊËÌÍÎÏÑÒÓÔÕÖÙÚÛÜÝřŘšŠťŤďĎňŇĚÉžŽčČ ';
  var outChrs = 'aaaaaceeeeeeiiiinooooouuuuyyAAAAACEEEEIIIINOOOOOUUUUYrRsStTdDnNEEzZcC_';
  var charsRgx = new RegExp('[' + inChrs + ']', 'g');
  var transl = {};
  var i;
  var lookup = function (m) { return transl[m] || m; };

  for (i = 0; i < inChrs.length; i++) {
    transl[ inChrs[i] ] = outChrs[i];
  }

  return function (string) { return string.replace(charsRgx, lookup); };
})();

function getServerMacAddress() {
  return getmac().toUpperCase();
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


export default {
  execPromise,
  stripAccents,
  getServerMacAddress,
  getServerHostname,
  getServerDomain,
  getServerDomainHostname,
  getServerUpdatesCount,
};
