import mongoose from 'mongoose';

const cfDeviceSchema = new mongoose.Schema({
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
    uppercase: true,
    set: function(device_id) {
      this._device_id = this.device_id;
      return device_id;
    },
  },
  mac_address: {
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    uppercase: true,
    mixLength: 12,
    maxLength: 17,
    set: function(mac_address) {
      this._mac_address = this.mac_address;
      return mac_address;
    },
  },
  car: { // TODO - hooks to handle other side
    type: mongoose.Types.ObjectId,
    ref: 'cf_car',
    unique: true,
    sparse: true,
    set: function(car) {
      this._car = this.car;
      return car;
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
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

export default mongoose.model('cf_device', cfDeviceSchema);
