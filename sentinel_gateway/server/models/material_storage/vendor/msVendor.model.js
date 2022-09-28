import mongoose from 'mongoose';

const msVendorSchema = new mongoose.Schema({
  vendor_code: {
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    set: function(vendor_code) {
      this._vendor_code = this.vendor_code;
      return vendor_code;
    },
  },
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
  description: {
    type: String,
    trim: true,
    set: function(description) {
      this._description = this.description;
      return description;
    },
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

export default mongoose.model('ms_vendor', msVendorSchema);
