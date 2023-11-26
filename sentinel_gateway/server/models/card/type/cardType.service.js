import CardTypeModel from './cardType.model.js';
import cardTypeInitData from './cardType.init.js';
import _ from 'underscore';

async function init() {
  _.each(cardTypeInitData, async (data) => {
    var itemExist = await CardTypeModel.exists({ name: data.name, is_secure: data.is_secure });
    if (!itemExist) {
      var newItem = new CardTypeModel({
        name: data.name,
        description: data.description,
        is_secure: data.is_secure,
        frequency: data.frequency,
        is_protected: data.is_protected
      });
      await newItem.save();
    }
  });
};

export default {
  init,
};
