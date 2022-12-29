import mongoose from 'mongoose';

const cfCarSchema = new mongoose.Schema({
  
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

export default mongoose.model('cf_car', cfCarSchema);
