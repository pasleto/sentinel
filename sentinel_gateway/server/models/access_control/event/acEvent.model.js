import mongoose from 'mongoose';

const acEventSchema = new mongoose.Schema({
  event_id: {
    type: Number,
    required: true,
    unique: true,
    set: function(event_id) {
      this._event_id = this.event_id;
      return event_id;
    },
  },
  initiator_type: {
    type: String,
    trim: true,
    required: true,
    // enum: ['RTC', 'Door Sensor', 'Reader', 'Button'],
    enum: {
      values: ['RTC', 'Door Sensor', 'Reader', 'Button'],
      message: '{VALUE} is not supported'
    },
    set: function(initiator_type) {
      this._initiator_type = this.initiator_type;
      return initiator_type;
    },
  },
  friendly_name: {
    type: String,
    trim: true,
    required: true,
    unique: true,
    set: function(friendly_name) {
      this._friendly_name = this.friendly_name;
      return friendly_name;
    },
  },
  status: {
    type: String,
    trim: true,
    required: true,
    // enum: ['success', 'info', 'warning', 'error'],
    enum: {
      values: ['success', 'info', 'warning', 'error'],
      message: '{VALUE} is not supported'
    },
    set: function(status) {
      this._status = this.status;
      return status;
    },
  },
  description: {
    type: String,
    trim: true,
    required: true,
    unique: true,
    set: function(description) {
      this._description = this.description;
      return description;
    },
  },
  is_protected: {
    type: Boolean,
    required: true,
    default: true,
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

// acEventSchema.index({ scope: 1, name: 1 }, { unique: true });

export default mongoose.model('ac_event', acEventSchema);
