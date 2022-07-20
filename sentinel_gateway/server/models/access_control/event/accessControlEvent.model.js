import mongoose from 'mongoose';

const accessControlEventSchema = new mongoose.Schema({
  event_id: {
    type: Number,
    required: true,
    unique: true,
  },
  initiator_type: {
    type: String,
    trim: true,
    required: true,
    enum: ['RTC', 'Door Sensor', 'Reader', 'Button'],
  },
  friendly_name: {
    type: String,
    trim: true,
    required: true,
    unique: true,
  },
  status: {
    type: String,
    trim: true,
    required: true,
    enum: ['success', 'info', 'warning', 'error'],
  },
  description: {
    type: String,
    trim: true,
    required: true,
    unique: true,
  },
  protected: {
    type: Boolean,
    required: true,
    default: true,
  }
}, {
  timestamps: true,
});

accessControlEventSchema.set('toJSON', { virtuals: true });

// accessControlEventSchema.index({ scope: 1, name: 1 }, { unique: true });

export default mongoose.model('accessControlEvent', accessControlEventSchema);
