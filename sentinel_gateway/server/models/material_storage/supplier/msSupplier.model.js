import mongoose from 'mongoose';

const msSupplierSchema = new mongoose.Schema({
  supplier_code: {
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    set: function(supplier_code) {
      this._supplier_code = this.supplier_code;
      return supplier_code;
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
  },
  address_street: {
    type: String,
    trim: true,
    set: function(address_street) {
      this._address_street = this.address_street;
      return address_street;
    },
  },
  address_city: {
    type: String,
    trim: true,
    set: function(address_city) {
      this._address_city = this.address_city;
      return address_city;
    },
  },
  address_postal_code: {
    type: String,
    trim: true,
    set: function(address_postal_code) {
      this._address_postal_code = this.address_postal_code;
      return address_postal_code;
    },
  },
  address_country: { // ? - maybe separated table - msCountry
    type: String,
    trim: true,
    set: function(address_country) {
      this._address_country = this.address_country;
      return address_country;
    },
  },
  contact_name: {
    type: String,
    trim: true,
    set: function(contact_name) {
      this._contact_name = this.contact_name;
      return contact_name;
    },
  },
  contact_phone: {
    type: String,
    trim: true,
    set: function(contact_phone) {
      this._contact_phone = this.contact_phone;
      return contact_phone;
    },
  },
  contact_email: {
    type: String,
    trim: true,
    set: function(contact_email) {
      this._contact_email = this.contact_email;
      return contact_email;
    },
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

export default mongoose.model('ms_supplier', msSupplierSchema);
