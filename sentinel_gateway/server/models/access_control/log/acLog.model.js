import mongoose from 'mongoose';

const acLogSchema = new mongoose.Schema({
  unit: {
    type: Object, // ObjectId
  },
  event: {
    type: Object, // ObjectId
  },
  initiator: {
    type: Object, // ObjectId
  },
  lock_id: {
    type: Number,
  },
  time_stamp: {
    type: Date,
  },
  card: {
    type: Object, // ObjectId
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

export default mongoose.model('ac_log', acLogSchema);
