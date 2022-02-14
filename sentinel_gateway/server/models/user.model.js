const mongoose = require('mongoose');
const Schema = mongoose.Schema;

/*
  Application User
*/

const userSchema = new Schema({ // TODO
  username: {
    type: String,
    trim: true,
    lowercase: true,
    required: true,
    unique: true,
  },
  password: {
    type: String,
    trim: true,
    required: true,
  },
}, {
  timestamps: true,
});

userSchema.set('toJSON', { virtuals: true });

module.exports = mongoose.model('User', userSchema);
