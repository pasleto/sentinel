const mongoose = require('mongoose');
const Schema = mongoose.Schema;
/*
  Card
*/
const cardSchema = new Schema({ // TODO
  uid_hex: {
    type: String,
    trim: true,
    required: true,
    unique: true,
  },
  // uid_dec: {
  //   type: Number,
  //   trim: true,
  //   unique: true,
  // },
  description: {
    type: String,
    trim: true,
    default: '',
  },
  employee: {
    type: mongoose.Types.ObjectId,
    ref: 'Employee',
    default: null,
  },
  has_limited_validity: {
    type: Boolean,
    required: true,
    default: false, // TODO - can specifiy if card is valid from date to date
  },
  valid_since: {
    type: Date,
  },
  valid_until: {
    type: Date,
  },
  is_active: {
    type: Boolean,
    required: true,
    default: false, // TODO - make active after assigned to employee
    // default: function () { return this.employee },
  },
  is_returned: {
    type: Boolean,
    required: true,
    default: false, // TODO - true after card has been returned
    // default: function () { return this.employee },
  },
  // is_primary: { // TODO - one primary card per user -> validation error if user already have primary card
  //   type: Boolean,
  //   required: true,
  //   default: false,
  // }
  
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true}
});

// function fillZerosToMaxLength(string) {
//   return string.padStart(12,'0');  // 12 - length
// }
// function hexToDec(hex) {
//   return parseInt(hex, 16);
// }

// cardSchema.pre('save', function(next) {
//   this.uid_dec = parseInt(this.uid_hex, 16);
//   next();
// });

module.exports = mongoose.model('Card', cardSchema);
