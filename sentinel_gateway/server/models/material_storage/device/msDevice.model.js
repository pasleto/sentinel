import mongoose from 'mongoose';
import { msGroupService } from '../../../controllers/mongo.controller.js';

// TODO - implement last_connected_timestamp (on connection or on disconnection ?)

const msDeviceSchema = new mongoose.Schema({
  name: {
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    set: function(name) {
      this._name = this.name;
      return name;
    },
  },
  device_id: {
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    set: function(device_id) {
      this._device_id = this.device_id;
      return device_id;
    },
  },
  hostname: {
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    set: function(hostname) {
      this._hostname = this.hostname;
      return hostname;
    },
  },
  description: {
    type: String,
    trim: true,
    set: function(description) {
      this._description = this.description;
      return description;
    },
  },
  mac_address: {
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    set: function(mac_address) {
      this._mac_address = this.mac_address;
      return mac_address;
    },
  },
  ip_address: { // TODO - is this needed? -> remove or rename to last_ip_address | or maybe needed for initial staging of device -> possible handle in one time operation?
    type: String,
    trim: true,
    set: function(ip_address) {
      this._ip_address = this.ip_address;
      return ip_address;
    },
  },
  groups: [{ // group - array - groups this device has assigned - one device can have multiple groups
    type: mongoose.Types.ObjectId,
    ref: 'ms_group',
    set: function(group) {
      this._group = this.group;
      return group;
    },
  }],

}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});


msDeviceSchema.pre('remove', { document: true, query: false }, async function(next) { // document middleware - pre - remove
  if (this.groups && this.groups.length > 0) { // before remove, check if device has any groups assigned, if so, remove this device reference from those groups
    this.groups.forEach(async (item) => {
      var group = await msGroupService.exists({_id: item});
      if (group) {
        group.devices.pull(this._id);
        await group.save();
      }
    });
  }
  // TODO - do the same thing in reverse - on remove group, remove reference on device

  next();
});


export default mongoose.model('ms_device', msDeviceSchema);
