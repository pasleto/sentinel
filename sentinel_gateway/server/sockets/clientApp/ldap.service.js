import clientApp from './clientApp.socket.js';
import ldap from '../../controllers/ldap.controller.js';

// socket can be accessed by this in classic funtion

async function ldapTestConnection(payload, callback) {
  // console.log(clientApp.getSocket(this.id)); // TODO
  console.log(global.connectedClientApp); // TODO

  await ldap.testConnection((result) => {
    callback(result);
  });
};

async function ldapSyncFull(payload, callback) { // TODO - without acknowledge, send start and end to admin room clients
  await ldap.allUserSync((result) => { // TODO
    callback(result);
  });
};

async function ldapSyncOne(payload, callback) {
  await ldap.oneUserSync((result) => { // TODO
    callback(result);
  });
};

export default {
  ldapTestConnection,
  ldapSyncFull,
  ldapSyncOne,
}
