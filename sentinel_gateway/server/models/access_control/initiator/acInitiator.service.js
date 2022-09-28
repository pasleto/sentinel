import AcInitiatorModel from './acInitiator.model.js';
import acInitiatorInitData from './acInitiator.init.js';
import _ from 'underscore';


async function init() {
  _.each(acInitiatorInitData, async (data) => {
    var itemExist = await AcInitiatorModel.exists({ initiator_id: data.initiator_id });
    if (!itemExist) { 
      var newItem = new AcInitiatorModel({ 
        initiator_id: data.initiator_id,
        initiator_type: data.initiator_type,
        friendly_name: data.friendly_name, 
        has_direction: data.has_direction, 
        direction: data.has_direction ? data.direction : undefined,
        is_protected: data.is_protected
      });
      await newItem.save();
    }
  });
};


export default {
  init,
};
