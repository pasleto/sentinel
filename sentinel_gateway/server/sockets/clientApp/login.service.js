// import clientApp from './clientApp.socket.js';
// import auth from '../../controllers/auth.controller.js';

import login from '../../controllers/login.controller.js';

// socket can be accessed by this in classic funtion

async function getUserData(payload, callback) { // TODO
  try {
    await login.clientAppGetUserData(this.handshake.auth.token.split(' ')[1], (result) => {
      if (result.status == 'OK') {
        // clientApp.socketLogin(this.id, result.data.user.username, result.data.user.id);
        if (result.data.user.is_admin) {
          console.log('joining admin room'); // TODO
          this.join('admin');
        }
        callback(result);
      } else {
        callback(result);
      }
    });
  } catch (error) {
    callback({
      status: 'NOK',
      data: {
        message: error.message
      }
    });
  }
};

// async function loginPassword(payload, callback) {
//   var socketObj = clientApp.getSocket(this.id);
//     if (socketObj && socketObj.is_logged) {
//       callback({
//         status: 'NOK',
//         data: {
//           message: 'Already logged in!'
//         }
//       });
//     } else {
//       await auth.clientAppPasswordAuthenticate(payload.username, payload.password, (result) => {
//         if (result.status == 'OK') {
//           clientApp.socketLogin(this.id, result.data.user.username, result.data.user.id);
//           if (result.data.user.is_admin) {
//             this.join('admin');
//           }
//           callback(result);
//         } else {
//           callback(result);
//         }
//       });
//     }
// };

// function logout(payload, callback) {
//   clientApp.socketLogout(this, (msg) => callback(msg));
// };

export default {
  getUserData
  // loginPassword,
  // logout,
}
