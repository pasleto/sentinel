import TimezoneModel from './timezone.model.js';
import timezoneData from './timezone.data.js';
import _ from 'underscore';

async function timezoneModelInit() {
  _.each(timezoneData, async (data) => {
    var itemExist = await TimezoneModel.findOne({ name: data.name });
    if (!itemExist) { 
      var newItem = new TimezoneModel({ 
        name: data.name, 
        friendly_name: data.friendly_name, 
        esp_value: data.esp_value,
        linux_value: data.linux_value,
        is_active: data.is_active,
        protected: data.protected,
      });
      await newItem.save();
    }
  });
};

export default timezoneModelInit;
