import mongoose from 'mongoose';

const msMaterialSchema = new mongoose.Schema({
  material_code: { // for barcode generation
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    set: function(material_code) {
      this._material_code = this.material_code;
      return material_code;
    },
  },
  product: {
    type: mongoose.Types.ObjectId,
    ref: 'ms_product',
    required: true,
    set: function(product) {
      this._product = this.product;
      return product;
    },
  },
  supplier: { // one product can be supplied by multiple suppliers with different prices - but it must be different record because of product id 
    type: mongoose.Types.ObjectId,
    ref: 'ms_supplier',
    required: true,
    set: function(supplier) {
      this._supplier = this.supplier;
      return supplier;
    },
  },
  unit_price: { // TODO - brutto, netto
    type: Number,
    required: true,
    // min: 0.1,
    min: [0.1, 'Must be at least 0.1'],
    set: function(unit_price) {
      this._unit_price = this.unit_price;
      return unit_price;
    },
  },
  price_currency: {
    type: mongoose.Types.ObjectId,
    ref: 'ms_currency',
    required: true,
    set: function(price_currency) {
      this._price_currency = this.price_currency;
      return price_currency;
    },
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

msMaterialSchema.index({ product: 1, supplier: 1 }, { unique: true }); // one product can be supplied by one supplier only once

export default mongoose.model('ms_material', msMaterialSchema);
