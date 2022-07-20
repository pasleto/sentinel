import mongoose from 'mongoose';

const accessControlInitiatorSchema = new mongoose.Schema({
  initiator_id: {
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
  has_direction: {
    type: Boolean,
    required: true,
  },
  direction: {
    type: String,
    trim: true,
    enum: ['Entry', 'Exit'],
    required: function() {
      return this.has_direction;
    },
  },
  protected: {
    type: Boolean,
    required: true,
    default: true,
  }
}, {
  timestamps: true,
});

accessControlInitiatorSchema.set('toJSON', { virtuals: true });

export default mongoose.model('accessControlInitiator', accessControlInitiatorSchema);
