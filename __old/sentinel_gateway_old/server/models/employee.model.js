const mongoose = require('mongoose');
const Schema = mongoose.Schema;

/*
  Company Employee
*/

const employeeSchema = new Schema({ // TODO
  name: {
    type: String,
    trim: true,
    required: true,
    unique: true,
  },
  cards: [{
    type: mongoose.Types.ObjectId,
    ref: 'Card'
  }],


  // email: {
  //   type: String,
  //   // `email` must be unique, unless it isn't defined
  //   index: { unique: true, sparse: true }
  // }
  
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true}
});

module.exports = mongoose.model('Employee', employeeSchema);
