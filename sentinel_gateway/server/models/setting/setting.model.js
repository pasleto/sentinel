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
  value: {
    type: String,
    trim: true,
    required: function () { return typeof this.value === 'string' ? false : true }, // allow empty string pass required
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
