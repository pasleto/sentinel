import mongoose from 'mongoose';

const accessControlLogSchema = new mongoose.Schema({
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
});

accessControlLogSchema.set('toJSON', { virtuals: true });

export default mongoose.model('accessControlLog', accessControlLogSchema);
