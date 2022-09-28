import MsCurrencyModel from './msCurrency.model.js';
import msCurrencyInitData from './msCurrency.init.js';
import _ from 'underscore';

async function init() {
  _.each(msCurrencyInitData, async (data) => {
    var itemExist = await MsCurrencyModel.exists({ code: data.code });
    if (!itemExist) { 
      var newItem = new MsCurrencyModel({ 
        name: data.name,
        code: data.code,
        symbol: data.symbol
      });
      await newItem.save();
    }
  });
};

export default {
  init,
};
