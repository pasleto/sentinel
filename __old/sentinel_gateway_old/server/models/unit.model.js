const mongoose = require('mongoose');
const Schema = mongoose.Schema;

/*
  Unit
*/

const unitSchema = new Schema({ // TODO
  name: {
    type: String,
    trim: true,
    required: true,
    unique: true,
  },
  mac_address: { // validate only for MAC string
    type: String,
    trim: true,
    required: true,
    unique: true,
    // match: 
    // validate: { 

    // }
  },
  ip_address: { // validate only for IP string
    type: String,
    trim: true,
    required: true,
    unique: true,
    // match:
    // validate: {
      
    // }
  },
  hostname: {
    type: String,
    trim: true,
    // required: true,
    unique: true,
  },
  drive_size_total: {
    type: String,
    trim: true,
    // required: true,
  },
  drive_size_free: {
    type: String,
    trim: true,
    // required: true,
  },
  drive_size_used: {
    type: String,
    trim: true,
    // required: true,
  },
  drive_usage_percentage: {
    type: String,
    trim: true,
    // required: true,
  },
  type: {
    type: mongoose.Types.ObjectId,
    ref: 'UnitType',
    required: true,
  },
  
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true}
});

module.exports = mongoose.model('Unit', unitSchema);
