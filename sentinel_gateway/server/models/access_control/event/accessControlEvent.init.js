import AccessControlEventModel from './accessControlEvent.model.js';
import accessControlEventData from './accessControlEvent.data.js';
import _ from 'underscore';

async function accessControlEventModelInit() {
  _.each(accessControlEventData, async (data) => {
    var itemExist = await AccessControlEventModel.findOne({ event_id: data.event_id });
    if (!itemExist) { 
      var newItem = new AccessControlEventModel({ 
        event_id: data.event_id,
        initiator_type: data.initiator_type,
        friendly_name: data.friendly_name, 
        status: data.status, 
        description: data.description,
        protected: data.protected
      });
      await newItem.save();
    }
  });
};

export default accessControlEventModelInit;
