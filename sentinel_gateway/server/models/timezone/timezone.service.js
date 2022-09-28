import TimezoneModel from './timezone.model.js';
import timezoneInitData from './timezone.init.js';
import _ from 'underscore';

async function init() {
  _.each(timezoneInitData, async (data) => {
    var itemExist = await TimezoneModel.exists({ name: data.name });
    if (!itemExist) { 
      var newItem = new TimezoneModel({ 
        name: data.name, 
        friendly_name: data.friendly_name, 
        esp_value: data.esp_value,
        linux_value: data.linux_value,
        is_protected: data.is_protected
      });
      await newItem.save();
    }
  });
};

export default {
  init,
};
