import MsMeasureUnitModel from './msMeasureUnit.model.js';
import msMeasureUnitInitData from './msMeasureUnit.init.js';
import _ from 'underscore';

async function init() {
  _.each(msMeasureUnitInitData, async (data) => {
    var itemExist = await MsMeasureUnitModel.exists({ code: data.code });
    if (!itemExist) { 
      var newItem = new MsMeasureUnitModel({ 
        name: data.name,
        code: data.code
      });
      await newItem.save();
    }
  });
};

export default {
  init,
};
