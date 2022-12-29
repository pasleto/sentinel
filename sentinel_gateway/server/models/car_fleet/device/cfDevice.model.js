import mongoose from 'mongoose';

const cfDeviceSchema = new mongoose.Schema({
  
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

export default mongoose.model('cf_device', cfDeviceSchema);
