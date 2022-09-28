import mongoose from 'mongoose';

const msSupplySchema = new mongoose.Schema({
  group: {
    type: mongoose.Types.ObjectId,
    ref: 'ms_group',
    required: true,
    set: function(group) {
      this._group = this.group;
      return group;
    },
  },
  material: {
    type: mongoose.Types.ObjectId,
    ref: 'ms_material',
    required: true,
    set: function(material) {
      this._material = this.material;
      return material;
    },
  },
  amount: {
    type: Number,
    required: true,
    min: 0,
    set: function(amount) {
      this._amount = this.amount;
      return amount;
    },
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});


export default mongoose.model('ms_supply', msSupplySchema);
