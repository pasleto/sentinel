const mongoose = require('mongoose');
const Schema = mongoose.Schema;

// https://stackoverflow.com/questions/42374422/mongoose-set-default-value-in-a-required-field-when-this-is-not-present-in-the/43870984

const settingSchema = new Schema({
  name: {
    type: String,
    trim: true,
    required: true,
    unique: true,
  },
  value: {
    type: String,
    trim: true,
    required: function () { return typeof this.value === 'string' ? false : true }, // allow empty string pass required
    // cast: false, // disable autocasting type
    // required: [
    //   function () { return typeof this.value === 'string' ? false : true },
    //   'your custom message here'
    // ]
    // required: isValueRequired
  },
  description: {
    type: String,
    trim: true,
    default: '',
  },
  protected: {
    type: Boolean,
    required: true,
    default: true,
  }
}, {
  timestamps: true,
});

// function isValueRequired () { return typeof this.value === 'string' ? false : true } // allow empty string pass required
// settingSchema.path('value').required(function () { return typeof this.value === 'string' ? false : true }, 'your custom message here');

settingSchema.set('toJSON', { virtuals: true });

module.exports = mongoose.model('Setting', settingSchema);
