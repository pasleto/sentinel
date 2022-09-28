import AcEventModel from './acEvent.model.js';
import acEventInitData from './acEvent.init.js';
import _ from 'underscore';


async function init() {
  _.each(acEventInitData, async (data) => {
    var itemExist = await AcEventModel.exists({ event_id: data.event_id });
    if (!itemExist) { 
      var newItem = new AcEventModel({ 
        event_id: data.event_id,
        initiator_type: data.initiator_type,
        friendly_name: data.friendly_name, 
        status: data.status, 
        description: data.description,
        is_protected: data.is_protected
      });
      await newItem.save();
    }
  });
};


export default {
  init,
};
