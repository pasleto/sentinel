const mongoose = require('mongoose');
const Schema = mongoose.Schema;

/*
  Company Department
*/

// https://mongoosejs.com/docs/populate.html#query-conditions

const departmentSchema = new Schema({ // TODO
  name: {
    type: String,
    trim: true,
    required: true,
    unique: true,
  },
  code: { // function to fill 0 up to maxlength
    type: String,
    minlength: 4,
    maxlength: 8,
    trim: true,
    unique: true,
  },
  responsible_person: {
    type: mongoose.Types.ObjectId,
    ref: 'Employee',
  },
  employees: [{
    type: mongoose.Types.ObjectId,
    ref: 'Employee',
  }],

  is_root: {
    type: Boolean,
    required: true,
    default: false,
  },
  parent: {
    type: mongoose.Types.ObjectId,
    required: function () { return !this.is_root },
    ref: 'Department',
  },
  is_protected: {
    type: Boolean,
    required: true,
    default: false,
  },
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true}
});

module.exports = mongoose.model('Department', departmentSchema);
