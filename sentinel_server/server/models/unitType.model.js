const mongoose = require('mongoose');
const Schema = mongoose.Schema;
/*
  Unit Type
*/
const unitTypeSchema = new Schema({
  name: {
    type: String,
    trim: true,
    required: true,
    unique: true,
  },
  description: {
    type: String,
    trim: true,
    default: '',
  },
  is_protected: {
    type: Boolean,
    required: true,
    default: false,
  },
  is_root: {
    type: Boolean,
    required: true,
    default: false,
  },
  parent: {
    type: mongoose.Types.ObjectId,
    required: function () { return !this.is_root },
    ref: 'UnitType',
  },
  image: {
    type: String,
    trim: true,
    default: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAMAAACdt4HsAAAAgVBMVEUAAABDQz8AAAADAwMAAAACAgIFBQUAAAAEBAQAAAAAAAAAAAAAAAABAQEAAAAGBgYAAAABAQEAAAARERAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAQEAAAABAQEAAAAAAAABAQECAgEAAAAAAAAKCgkBAQEAAAABAQEBAQEAAABUy2OkAAAAKnRSTlMAAt4e5icf+xOh7WHOHDIPxyvcA+qc0/Nsw3NCOLY2p5MlGIl7CoBlWFazzkpTAAACL0lEQVRYw+2X7XKiMBSGTwAlKiEKdPkq9bu7m/u/wI0xuyebaHvAdqY/elQcg8875BlCEgDGgE0/PMzrhC/Ej6lQAjAp4jiex6SDkAw8XjZtEZGraDuOnTD8U6pGVbrl/0lokKdWBw4vW91S0fuQ6L+vJUoAUSiV9IIqUfQ6IdkDSowjpSIB5BKRDpihhGtATL//YxOAEmwAfWDYAJQ4vwTMybwNQInYBQKPV+BLjOkDa/6pEjH3IYn5cdEJmCwRet2gBgETJcJ5sONlokQ4FCZgMVmiXJuAI0yW2NSaj/LpEs/Nuhh69sidKMUZf15NkSXadpff7HYbmD6cIc+UynIYITHkdcILXWLI2wSyxJC3CTSJIZ9ut6n++vECJIkhv+B8YRMIEvEE8gA24ZkgUXZPv0uPZ5jwrkS5S1XdlODw5gQm3JOIvDIJHo8Jb0jEiV4n+DwmiPsSueEvn+XR59HDr+KORMu3p0rp8nk0mdS3JVp+vYHVMuRdk+qmROSBXRPag89jQihRSOShNAnVqvR4TAgkFrnDM3sNyxUD5F2TocRkcHj9/peAKJpUocRrIQ94DeFzYpYEEgOeYS9K2sQS8lBiAmFi8XjPJGFi8XjPJGFiqZ5dPjRJmZ3DuiRU1gNWINEsdate3F7YnupLwk/nrOgrs9T1F9vRnTJ3bu00FJpXGXd3HN345f6rywPfjuWHw1/WDvNTloyo7NXj9YvvZ+TacwYfte373jsD/AFqRYLo93DP1wAAAABJRU5ErkJggg==',
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true}
});

module.exports = mongoose.model('UnitType', unitTypeSchema);
