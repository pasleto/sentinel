import mongoose from 'mongoose';

// TODO - add permissions

const msGroupSchema = new mongoose.Schema({
  name: {
    type: String,
    reguired: true,
    unique: true,
    trim: true,
    set: function(name) {
      this._name = this.name;
      return name;
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
  supplies: [{ // material - array - material list in this group with amount in stock
    type: mongoose.Types.ObjectId,
    ref: 'ms_supply',
    set: function(supply) {
      this._supply = this.supply;
      return supply;
    },
  }],
  devices: [{ // device - array - devices where this group is assigned - one area can have multiple terminals
    type: mongoose.Types.ObjectId,
    ref: 'ms_device',
    set: function(device) {
      this._device = this.device;
      return device;
    },
  }],
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});


export default mongoose.model('ms_group', msGroupSchema);
