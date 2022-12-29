import mongoose from 'mongoose';

const cfTestSchema = new mongoose.Schema({
  unit_id: {
    type: String,
    trim: true,
  },
  time: {
    type: Date,
  },
  temp_int: {
    type: Number,
  },
  gps_signal: {
    type: Boolean,
  },
  gps_lat: {
    type: Number,
  },
  gps_lon: {
    type: Number,
  },
  gps_alt: {
    type: Number,
  },
  gps_speed: {
    type: Number,
  },
  gps_accuracy: {
    type: Number,
  },
  gps_time: {
    type: Date,
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

export default mongoose.model('cf_test', cfTestSchema);
