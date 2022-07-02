import mongoose from 'mongoose';

const timezoneSchema = new mongoose.Schema({
  name: {
    type: String,
    trim: true,
    required: true,
    unique: true,
  },
  friendly_name: {
    type: String,
    trim: true,
    required: true,
    unique: true,
  },
  esp_value: {
    type: String,
    trim: true,
    required: true,
  },
  linux_value: {
    type: String,
    trim: true,
    required: true,
  },
  is_active: {
    type: Boolean,
    required: true,
    default: false,
  },
  protected: {
    type: Boolean,
    required: true,
    default: true,
  }
}, {
  timestamps: true,
});

timezoneSchema.set('toJSON', { virtuals: true });

export default mongoose.model('timezone', timezoneSchema);
