import mongoose from 'mongoose';

const acInitiatorSchema = new mongoose.Schema({
  initiator_id: {
    type: Number,
    required: true,
    unique: true,
    set: function(initiator_id) {
      this._initiator_id = this.initiator_id;
      return initiator_id;
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
  has_direction: {
    type: Boolean,
    required: true,
    set: function(has_direction) {
      this._has_direction = this.has_direction;
      return has_direction;
    },
  },
  direction: {
    type: String,
    trim: true,
    // enum: ['Entry', 'Exit'],
    enum: { 
      values: ['Entry', 'Exit'], 
      message: '{VALUE} is not supported' 
    },
    required: function() { return this.has_direction },
    set: function(direction) {
      this._direction = this.direction;
      return direction;
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

export default mongoose.model('ac_initiator', acInitiatorSchema);
