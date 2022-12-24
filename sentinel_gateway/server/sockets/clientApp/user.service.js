import clientApp from './clientApp.socket.js';
import utils from '../../utils/util.js';
import QRCode from 'qrcode';
import { userService, cardService } from '../../controllers/mongo.controller.js';

// socket can be accessed by this in classic funtion

async function userCardSearch(payload, callback) {
  try {
    var cardExist = await cardService.getOne({ value: payload.value });
    var existingUser = await userService.getOne(false, false, { _id: cardExist.user }, { user_number: 1, name: 1, surname: 1, username: 1 });
    callback({ 
      status: 'OK', 
      data: {
        card: cardExist,
        user: existingUser
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

async function userCardCreate(payload, callback) {
  try {
    var socketObj = clientApp.getSocket(this.id);
    var cardExist = await cardService.exists({ value: payload.value });
    if (cardExist) {
      var existingUser = await userService.getOne(false, false, { _id: cardExist.user }, { user_number: 1, name: 1, surname: 1, username: 1 });
      if (payload.force) {
        var newUser = await userService.getOne(false, false, { _id: payload.user }, { user_number: 1, name: 1, surname: 1, username: 1 });
        await cardService.modify(cardExist._id, {
          ...(payload.type != null && payload.type != undefined) && { type: payload.type },
          ...(payload.description != null && payload.description != undefined) && { description: payload.description },
          ...(payload.has_validity != null && payload.has_validity != undefined) && { has_validity: payload.has_validity },
          ...(payload.valid_from != null && payload.valid_from != undefined) && { valid_from: payload.valid_from },
          ...(payload.valid_until != null && payload.valid_until != undefined) && { valid_until: payload.valid_until },
          ...(payload.is_enabled != null && payload.is_enabled != undefined) && { is_enabled: payload.is_enabled },
          ...(payload.user != null && payload.user != undefined) && { user: newUser._id },
          last_changed_by: socketObj.user_id
        });
        callback({ 
          status: 'OK', 
          data: {
            message: 'Card Reassigned',
            old_user: existingUser,
            new_user: newUser
          } 
        });
      } else {
        callback({ 
          status: 'NOK', 
          data: {
            message: 'This card already exist',
            card: cardExist,
            user: existingUser
          } 
        });
      }
    } else {
      await cardService.create({
        last_changed_by: socketObj.user_id,
        value: payload.value,
        type: payload.type,
        is_enabled: payload.is_enabled,
        user: payload.user,
        has_validity: payload.has_validity,
        ...(payload.valid_from != null && payload.valid_from != undefined) && { valid_from: payload.valid_from },
        ...(payload.valid_until != null && payload.valid_until != undefined) && { valid_until: payload.valid_until },
        ...(payload.description != null && payload.description != undefined) && { description: payload.description }
      });
      callback({ 
        status: 'OK', 
        data: {
          message: 'Card Created',
        } 
      });
    }
  } catch (error) {
    callback({ 
      status: 'NOK', 
      data: {
        message: error.message
      } 
    }); 
  }

};

async function userCardEdit(payload, callback) { 
  try {
    var socketObj = clientApp.getSocket(this.id);
    await cardService.modify(payload.card_id, {
      ...(payload.value != null && payload.value != undefined) && { value: payload.value },
      ...(payload.type != null && payload.type != undefined) && { type: payload.type },
      ...(payload.description != null && payload.description != undefined) && { description: payload.description },
      ...(payload.has_validity != null && payload.has_validity != undefined) && { has_validity: payload.has_validity },
      ...(payload.valid_from != null && payload.valid_from != undefined) && { valid_from: payload.valid_from },
      ...(payload.valid_until != null && payload.valid_until != undefined) && { valid_until: payload.valid_until },
      ...(payload.is_enabled != null && payload.is_enabled != undefined) && { is_enabled: payload.is_enabled },
      ...(payload.user != null && payload.user != undefined) && { user: payload.user },
      last_changed_by: socketObj.user_id
    });
    callback({ 
      status: 'OK', 
      data: {
        message: 'Card Modified'
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

async function userCardDelete(payload, callback) {
  try {
    await cardService.remove(payload.id);
    callback({ 
      status: 'OK', 
      data: {
        message: 'Card Deleted'
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

async function userCardBarcodeGenerate(payload, callback) {
  try {
    var cardExist = await cardService.getOne({ _id: payload.card_id });
    var cryptedCard = await utils.cryptCard(cardExist.value);
    var barcodeImage = await QRCode.toDataURL(cryptedCard);
    callback({ 
      status: 'OK', 
      data: {
        crypted: cryptedCard,
        barcode: barcodeImage.split(',')[1]
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

async function userGet(payload, callback) { // TODO
  try {

    // TODO
    
    callback({ 
      status: 'OK', 
      data: {
        message: 'TODO'
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

async function userCreate(payload, callback) { // TODO
  try {

    // TODO
    
    callback({ 
      status: 'OK', 
      data: {
        message: 'TODO'
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

async function userEdit(payload, callback) { // TODO
  try {

    // TODO
    
    callback({ 
      status: 'OK', 
      data: {
        message: 'TODO'
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

async function userDelete(payload, callback) { // TODO
  try {

    // TODO
    
    callback({ 
      status: 'OK', 
      data: {
        message: 'TODO'
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

export default {
  userCardSearch,
  userCardCreate,
  userCardEdit,
  userCardDelete,
  userCardBarcodeGenerate,
  userGet,
  userCreate,
  userEdit,
  userDelete,
}
