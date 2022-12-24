import mongoose from 'mongoose';
import _ from 'underscore';
import utils from '../../utils/util.js';
import TimezoneModel from '../timezone/timezone.model.js'; // TODO - switch to service

const settingSchema = new mongoose.Schema({
  scope: {
    type: String,
    trim: true,
    required: true,
    default: 'general',
    set: function(scope) {
      this._scope = this.scope;
      return scope;
    },
  },
  name: {
    type: String,
    trim: true,
    required: true,
    set: function(name) {
      this._name = this.name;
      return name;
    },
  },
  friendly_name: {
    type: String,
    trim: true,
    required: true,
    set: function(friendly_name) {
      this._friendly_name = this.friendly_name;
      return friendly_name;
    },
  },
  value: {
    type: String,
    trim: true,
    required: function () { return typeof this.value === 'string' ? false : true }, // allow empty string pass required
    set: function(value) {
      this._value = this.value;
      return value;
    },
  },
  description: {
    type: String,
    trim: true,
    default: '',
    set: function(description) {
      this._description = this.description;
      return description;
    },
  },
  is_protected: {
    type: Boolean,
    required: true,
    default: true,
    set: function(is_protected) {
      this._is_protected = this.is_protected;
      return is_protected;
    },
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

settingSchema.index({ scope: 1, name: 1 }, { unique: true });

settingSchema.pre('save', { document: true, query: false }, function(next) { // document middleware - pre - save
  if (this.isModified('value') && this.scope == 'general' && this.name == 'hash_card' && this.isNew) {
    this.value = utils.generateRandomHash();
  }
  if (this.isModified('value') && this.scope == 'general' && this.name == 'hash_jwt' && this.isNew) {
    this.value = utils.generateRandomHash();
  }
  if (this.isModified('value') && this.scope == 'proxy' && this.name == 'url_string') { // crypt proxy url_string password + handle string change
    if (this.isNew) {
      if (this.value) this.value = utils.crypt(this.id, this.value);
    } else {
      this._wasModified = true;
      if (this.value) this.value = utils.crypt(this.id, this.value);
    }
  }
  if (this.isModified('value') && this.scope == 'general' && this.name == 'timezone' && this.value && !this.isNew) { // update active timezone when changed
    this._wasModified = true; // incoming is timezoneModel.name | maybe change to timezoneModel.id
  }
  if (this.isModified('value') && this.scope == 'ldap' && this.name == 'password' && this.value) { // crypt ldap user password
    this.value = utils.crypt(this.id, this.value);
  }
  if (this.isModified('value') && this.scope == 'smtp' && this.name == 'password' && this.value) { // crypt smtp user password
    this.value = utils.crypt(this.id, this.value);
  }
  next();
});

settingSchema.post('save', { document: true, query: false }, async function(doc, next) { // document middleware - post - save
  if (doc.scope == 'general' && doc.name == 'timezone' && doc._wasModified) { // on timezone change
    var tz = await TimezoneModel.findOne({name: doc.value}, { createdAt: 0, updatedAt: 0, __v: 0, is_protected: 0, _id: 0 }).lean();
    if (tz) {
      console.log(tz);
      await utils.setServerTimezone(tz.linux_value); // TODO - change timezone on server
      global.io.emit('timezone-change', tz); // global.io.emit // ? - validate - broadcast timezone change - to all socket io namespaces, all rooms, all connected
      // TODO - move global socket call to another service file
      // TODO - add service for get - return timezone record
    }
  }
  if (doc.scope == 'proxy' && doc.name == 'url_string' && doc._wasModified) {
    if (doc.value) {
      doc.value = utils.decrypt(doc.id, doc.value); // decrypt proxy string
      utils.setServerProxy(true, doc.value);
    } else {
      utils.setServerProxy(false, undefined);
    }
  }
  // console.log('Setting Saved: ' + doc);
  next();
});

// settingSchema.pre('find', function(next) {
//   console.log("Pre Find");
//   next();
// });

// settingSchema.pre('findOne', function(next) {
//   console.log("Pre Find One");
//   next();
// });

// settingSchema.post('find', function(doc, next) {
//   console.log("Post Find");
//   next();
// });

settingSchema.post('findOne', function(doc, next) { // query middleware - post - findOne // ? - fired on exists and database init -> no doc attributes || not working when filtering only value
  if (doc) {
    if (doc.scope == 'ldap' && doc.name == 'password') { // decrypt ldap user password on findOne
      if (doc.value) doc.value = utils.decrypt(doc.id, doc.value); // doc._doc.value = utils.decrypt(doc.id, doc.value);
    }
    if (doc.scope == 'smtp' && doc.name == 'password') { // decrypt smtp user password on findOne
      if (doc.value) doc.value = utils.decrypt(doc.id, doc.value); // doc._doc.value = utils.decrypt(doc.id, doc.value);
    }
    if (doc.scope == 'proxy' && doc.name == 'url_string') { // decrypt proxy url string on findOne
      if (doc.value) doc.value = utils.decrypt(doc.id, doc.value); // doc._doc.value = utils.decrypt(doc.id, doc.value);
    }
    // console.log("Post Find One: " + doc);
  }
  next();
});

export default mongoose.model('setting', settingSchema);
