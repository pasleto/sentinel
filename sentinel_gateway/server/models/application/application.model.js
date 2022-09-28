import mongoose from 'mongoose';

const applicationSchema = new mongoose.Schema({ // TODO
  name: {
    type: String,
    reguired: true,
    trim: true,
    set: function(name) {
      this._name = this.name;
      return name;
    },
  },

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

export default mongoose.model('application', applicationSchema);
