import AccessControlInitiatorModel from './accessControlInitiator.model.js';
import accessControlInitiatorData from './accessControlInitiator.data.js';
import _ from 'underscore';

async function accessControlInitiatorInit() {
  _.each(accessControlInitiatorData, async (data) => {
    var itemExist = await AccessControlInitiatorModel.findOne({ initiator_id: data.initiator_id });
    if (!itemExist) { 
      var newItem = new AccessControlInitiatorModel({ 
        initiator_id: data.initiator_id,
        initiator_type: data.initiator_type,
        friendly_name: data.friendly_name, 
        has_direction: data.has_direction, 
        direction: data.has_direction ? data.direction : undefined,
        protected: data.protected
      });
      await newItem.save();
    }
  });
};

export default accessControlInitiatorInit;
