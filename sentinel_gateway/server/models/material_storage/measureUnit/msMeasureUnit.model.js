import mongoose from 'mongoose';

const msMeasureUnitSchema = new mongoose.Schema({
  name: {
    type: String,
    required: true,
    trim: true,
    unique: true,
    set: function(name) {
      this._name = this.name;
      return name;
    },
  },
  code: {
    type: String,
    required: true,
    trim: true,
    unique: true,
    set: function(code) {
      this._code = this.code;
      return code;
    },
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

export default mongoose.model('ms_measureunit', msMeasureUnitSchema);
