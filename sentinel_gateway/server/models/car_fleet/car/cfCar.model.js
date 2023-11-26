import mongoose from 'mongoose';

const cfCarSchema = new mongoose.Schema({
  licence_plate_number: {
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    uppercase: true,
    set: function(licence_plate_number) {
      this._licence_plate_number = this.licence_plate_number;
      return licence_plate_number;
    },
  },
  vin_number: {
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    uppercase: true,
    mixLength: 11,
    maxLength: 17,
    set: function(vin_number) {
      this._vin_number = this.vin_number;
      return vin_number;
    },
  },
  brand: {
    type: String,
    trim: true,
    set: function(brand) {
      this._brand = this.brand;
      return brand;
    },
  },
  model: {
    type: String,
    trim: true,
    set: function(model) {
      this._model = this.model;
      return model;
    },
  },
  manufacture_year: {
    type: Number,
    min: 0,
    max: 9999,
    set: function(manufacture_year) {
      this._manufacture_year = this.manufacture_year;
      return manufacture_year;
    },
  },
  fuel_type: {
    type: String,
    enum: {
      values: ['Diesel', 'Petrol', 'Electric', 'LPG', 'CNG', 'Hybrid'], // ? separate file or table -> to be able to fetch in frontend
      message: '{VALUE} is not supported'
    },
    trim: true,
    set: function(fuel_type) {
      this._fuel_type = this.fuel_type;
      return fuel_type;
    },
  },
  purchase_date: {
    type: Date,
    set: function(purchase_date) {
      this._purchase_date = this.purchase_date;
      return purchase_date;
    },
  },
  decommission_date: {
    type: Date,
    set: function(decommission_date) {
      this._decommission_date = this.decommission_date;
      return decommission_date;
    },
  },
  tachometer_status: {
    type: Number,
    set: function(tachometer_status) {
      this._tachometer_status = this.tachometer_status;
      return tachometer_status;
    },
  },
  engine_kw: {
    type: Number,
    set: function(engine_kw) {
      this._engine_kw = this.engine_kw;
      return engine_kw;
    },
  },
  engine_ccm: {
    type: Number,
    set: function(engine_ccm) {
      this._engine_ccm = this.engine_ccm;
      return engine_ccm;
    },
  },
  device: { // TODO - hooks to handle other side
    type: mongoose.Types.ObjectId,
    ref: 'cf_device',
    unique: true,
    sparse: true,
    set: function(device) {
      this._device = this.device;
      return device;
    },
  },
  responsible_person: {
    type: mongoose.Types.ObjectId,
    ref: 'user',
    set: function(responsible_person) {
      this._responsible_person = this.responsible_person;
      return responsible_person;
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

export default mongoose.model('cf_car', cfCarSchema);
