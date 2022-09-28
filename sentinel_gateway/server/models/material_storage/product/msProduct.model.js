import mongoose from 'mongoose';

// TODO - product category ?

const msProductSchema = new mongoose.Schema({
  product_code: { // for product specification
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    set: function(product_code) {
      this._product_code = this.product_code;
      return product_code;
    },
  },
  name: {
    type: String,
    reguired: true,
    trim: true,
    set: function(name) {
      this._name = this.name;
      return name;
    },
  },
  description: {
    type: String,
    trim: true,
    set: function(description) {
      this._description = this.description;
      return description;
    },
  },
  vendor: {
    type: mongoose.Types.ObjectId,
    ref: 'ms_vendor',
    required: true,
    set: function(vendor) {
      this._vendor = this.vendor;
      return vendor;
    },
  },
  amount: {
    type: Number,
    required: true,
    // min: 0.1,
    min: [0.1, 'Must be at least 0.1'],
    set: function(amount) {
      this._amount = this.amount;
      return amount;
    },
  },
  measure_unit: {
    type: mongoose.Types.ObjectId,
    ref: 'ms_measureunit',
    required: true,
    set: function(measure_unit) {
      this._measure_unit = this.measure_unit;
      return measure_unit;
    },
  },
  image: {
    type: String,
    trim: true,
    default: 'PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIiA/Pgo8IURPQ1RZUEUgc3ZnIFBVQkxJQyAiLS8vVzNDLy9EVEQgU1ZHIDEuMS8vRU4iICJodHRwOi8vd3d3LnczLm9yZy9HcmFwaGljcy9TVkcvMS4xL0RURC9zdmcxMS5kdGQiPgo8c3ZnIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sbnM6eGxpbms9Imh0dHA6Ly93d3cudzMub3JnLzE5OTkveGxpbmsiIHZlcnNpb249IjEuMSIgd2lkdGg9IjUwMCIgaGVpZ2h0PSI1MDAiIHZpZXdCb3g9IjAgMCA1MDAgNTAwIiB4bWw6c3BhY2U9InByZXNlcnZlIj4KPGRlc2M+Q3JlYXRlZCB3aXRoIEZhYnJpYy5qcyA0LjYuMDwvZGVzYz4KPGRlZnM+CjwvZGVmcz4KPGcgdHJhbnNmb3JtPSJtYXRyaXgoMS4xNSAwIDAgMS4xNSAyNTAgMjUwKSIgaWQ9Incyckk3RTZGX08yWmhGTTVjOXJ4ZyIgID4KPGcgc3R5bGU9IiIgdmVjdG9yLWVmZmVjdD0ibm9uLXNjYWxpbmctc3Ryb2tlIiAgID4KCQk8ZyB0cmFuc2Zvcm09Im1hdHJpeCgxIDAgMCAxIDAgMCkiIGlkPSJVRHhYeE1KNzFyN2N3b09zeWJVOEYiICA+CjxwYXRoIHN0eWxlPSJzdHJva2U6IG5vbmU7IHN0cm9rZS13aWR0aDogMTsgc3Ryb2tlLWRhc2hhcnJheTogbm9uZTsgc3Ryb2tlLWxpbmVjYXA6IGJ1dHQ7IHN0cm9rZS1kYXNob2Zmc2V0OiAwOyBzdHJva2UtbGluZWpvaW46IG1pdGVyOyBzdHJva2UtbWl0ZXJsaW1pdDogNDsgZmlsbDogcmdiKDI0MywyNDcsMjUwKTsgZmlsbC1ydWxlOiBldmVub2RkOyBvcGFjaXR5OiAxOyIgdmVjdG9yLWVmZmVjdD0ibm9uLXNjYWxpbmctc3Ryb2tlIiAgdHJhbnNmb3JtPSIgdHJhbnNsYXRlKC0zNjAsIC0zNjApIiBkPSJNIDAgMCBMIDcyMCAwIEwgNzIwIDcyMCBMIDAgNzIwIHoiIHN0cm9rZS1saW5lY2FwPSJyb3VuZCIgLz4KPC9nPgoJCTxnIHRyYW5zZm9ybT0ibWF0cml4KDEgMCAwIDEgMCAwKSIgaWQ9InJjU1piOWVwWEtKblZBZVd3enFIMyIgID4KPHBhdGggc3R5bGU9InN0cm9rZTogbm9uZTsgc3Ryb2tlLXdpZHRoOiAxOyBzdHJva2UtZGFzaGFycmF5OiBub25lOyBzdHJva2UtbGluZWNhcDogYnV0dDsgc3Ryb2tlLWRhc2hvZmZzZXQ6IDA7IHN0cm9rZS1saW5lam9pbjogbWl0ZXI7IHN0cm9rZS1taXRlcmxpbWl0OiA0OyBmaWxsOiByZ2IoMTk3LDIxNiwyMzEpOyBmaWxsLXJ1bGU6IG5vbnplcm87IG9wYWNpdHk6IDE7IiB2ZWN0b3ItZWZmZWN0PSJub24tc2NhbGluZy1zdHJva2UiICB0cmFuc2Zvcm09IiB0cmFuc2xhdGUoLTM2MCwgLTM2MCkiIGQ9Ik0gMjE4Ljc1IDIxOC43NSBMIDIxOC43NSA1MDEuMjUgTCA1MDEuMjUgNTAxLjI1IEwgNTAxLjI1IDIxOC43NSBMIDIxOC43NSAyMTguNzUgeiBNIDQ4Mi41IDQ4Mi41IEwgMjM3LjUgNDgyLjUgTCAyMzcuNSA0NDUgTCA0ODIuNSA0NDUgTCA0ODIuNSA0ODIuNSB6IE0gMjM3LjUgNDI1LjYyNSBMIDIzNy41IDIzNy41IEwgNDgyLjUgMjM3LjUgTCA0ODIuNSA0MjUuNjI1IEwgMjM3LjUgNDI1LjYyNSB6IE0gNDA4LjEyNSAzMzMuNzUgTCA0NjcuNSAzOTEuMjUgTCA0NTQuMzc1IDQwNSBMIDQwOC43NSAzNjAuNjI1IEwgMzc5LjM3NSAzOTEuODc1IEwgMzMyLjUgMzA5LjM3NSBMIDI2OC43NSA0MDIuNSBMIDI1My4xMjUgMzkxLjg3NSBMIDMzNC4zNzUgMjczLjc1IEwgMzgzLjEyNSAzNjAuNjI1IEwgNDA4LjEyNSAzMzMuNzUgeiIgc3Ryb2tlLWxpbmVjYXA9InJvdW5kIiAvPgo8L2c+CjwvZz4KPC9nPgo8L3N2Zz4=',
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});


export default mongoose.model('ms_product', msProductSchema);
