import mongoose from 'mongoose';

const user = new mongoose.Schema({ // TODO
  username: {
    type: String,
  }
}, {
  timestamps: true,
});

user.set('toJSON', { virtuals: true });

export default mongoose.model('user', user);
