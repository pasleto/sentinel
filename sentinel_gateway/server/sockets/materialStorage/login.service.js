import materialStorage from './materialStorage.socket.js';
import auth from '../../controllers/auth.controller.js';

// socket can be accessed by this in classic funtion

async function loginPassword(payload, callback) {
  var socketObj = materialStorage.getSocket(this.id);
  if (socketObj && socketObj.is_logged) {
    callback({
      status: 'NOK',
      data: {
        message: 'Already logged in!'
      }
    });
  } else {
    await auth.materialStoragePasswordAuthenticate(payload.username, payload.password, this.handshake.headers.device, (result) => {
      if (result.status == 'OK') {
        materialStorage.socketLogin(this.id, result.data.user.username, result.data.user.id);
        callback(result);
      } else {
        callback(result);
      }
    });
  }
};

async function loginCard(payload, callback) {
  var socketObj = materialStorage.getSocket(this.id);
  if (socketObj && socketObj.is_logged) {
    callback({
      status: 'NOK',
      data: {
        message: 'Already logged in!'
      }
    });
  } else {
    await auth.materialStorageCardAuthenticate(payload.card, this.handshake.headers.device, (result) => {
      if (result.status == 'OK') {
        materialStorage.socketLogin(this.id, result.data.user.username, result.data.user.id);
        callback(result);
      } else {
        callback(result);
      }
    });
  }
};

async function loginBarcode(payload, callback) {
  var socketObj = materialStorage.getSocket(this.id);
  if (socketObj && socketObj.is_logged) {
    callback({
      status: 'NOK',
      data: {
        message: 'Already logged in!'
      }
    });
  } else {
    await auth.materialStorageCardBarcodeAuthenticate(payload.crypted_card, this.handshake.headers.device, (result) => {
      if (result.status == 'OK') {
        materialStorage.socketLogin(this.id, result.data.user.username, result.data.user.id);
        callback(result);
      } else {
        callback(result);
      }
    });
  }
};

function logout(payload, callback) {
  materialStorage.socketLogout(this, (msg) => callback(msg));
};

export default {
  loginPassword,
  loginCard,
  loginBarcode,
  logout,
}
