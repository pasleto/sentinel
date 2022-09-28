import mongoose from 'mongoose';

const cardTypeSchema = new mongoose.Schema({ // TODO
  name: {
    type: String,
    trim: true,
    required: true,
    set: function(name) {
      this._name = this.name;
      return name;
    },
    // get: getName,
  },
  description: {
    type: String,
    trim: true,
    set: function(description) {
      this._description = this.description;
      return description;
    },
  },
  is_secure: {
    type: Boolean,
    required: true,
    default: false,
    set: function(is_secure) {
      this._is_secure = this.is_secure;
      return is_secure;
    },
  },
  frequency: {
    type: String,
    trim: true,
    required: true,
    // enum: ['LF (125 KHz)', 'HF (13.56 MHz)'],
    enum: { 
      values: ['LF (125 KHz)', 'HF (13.56 MHz)'], 
      message: '{VALUE} is not supported' 
    },
    set: function(frequency) {
      this._frequency = this.frequency;
      return frequency;
    },
  },

  cards : [{ // TODO
    type: mongoose.Types.ObjectId,
    ref: 'card',
  }],

  is_protected: {
    type: Boolean,
    required: true,
    default: false,
    set: function(is_protected) {
      this._is_protected = this.is_protected;
      return is_protected;
    },
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

// function getName(value) {
//   if (this.is_secure) {
//     return '[SECURED] ' + value;
//   } else {
//     return '[CSN/UID] ' + value;
//   }
// }

cardTypeSchema.index({ name: 1, is_secure: 1 }, { unique: true });

export default mongoose.model('card_type', cardTypeSchema);
