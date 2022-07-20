import mongoose from 'mongoose';

const person = new mongoose.Schema({ // TODO
  name: {
    type: String,
  }
}, {
  timestamps: true,
});

person.set('toJSON', { virtuals: true });

export default mongoose.model('person', person);
