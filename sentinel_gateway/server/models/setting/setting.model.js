import mongoose from 'mongoose';

const settingSchema = new mongoose.Schema({
  scope: {
    type: String,
    trim: true,
    required: true,
    default: 'general',
  },
  name: {
    type: String,
    trim: true,
    required: true,
  },
  friendly_name: {
    type: String,
    trim: true,
    required: true,
  },
  value: { // TODO - allow multiple data types
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

settingSchema.set('toJSON', { virtuals: true });

settingSchema.index({ scope: 1, name: 1 }, { unique: true });

export default mongoose.model('setting', settingSchema);
