import mongoose from 'mongoose';
import _ from 'underscore';
import { userService } from '../../controllers/mongo.controller.js';

const cardSchema = new mongoose.Schema({
  value: {
    type: String,
    required: true,
    unique: true,
    set: function(value) {
      this._value = this.value;
      return value;
    },
  },
  type: {
    type: mongoose.Types.ObjectId,
    ref: 'card_type',
    required: true,
    set: function(type) {
      this._type = this.type;
      return type;
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
  has_validity: {
    type: Boolean,
    required: true,
    default: false,
    set: function(has_validity) {
      this._has_validity = this.has_validity;
      return has_validity;
    },
  },
  valid_since: {
    type: Date,
    required: function () { return this.has_validity },
    set: function(valid_since) {
      this._valid_since = this.valid_since;
      return valid_since;
    },
  },
  valid_until: {
    type: Date,
    required: function () { return this.has_validity },
    set: function(valid_until) {
      this._valid_until = this.valid_until;
      return valid_until;
    },
  },
  is_enabled: {
    type: Boolean,
    required: true,
    default: true,
    set: function(is_enabled) {
      this._is_enabled = this.is_enabled;
      return is_enabled;
    },
  },
  // assigned_by: {
  //   type: mongoose.Types.ObjectId,
  //   ref: 'user',
  //   required: true,
  //   set: function(assigned_by) {
  //     this._assigned_by = this.assigned_by;
  //     return assigned_by;
  //   },
  // },
  last_changed_by: {
    type: mongoose.Types.ObjectId,
    ref: 'user',
    required: true,
    set: function(last_changed_by) {
      this._last_changed_by = this.last_changed_by;
      return last_changed_by;
    },
  },
  user: { // user which this card is assigned to
    type: mongoose.Types.ObjectId,
    ref: 'user',
    required: true,
    set: function(user) {
      this._user = this.user;
      return user;
    },
  }
}, {
  timestamps: true,
  toObject: { virtuals: true },
  toJSON: { virtuals: true},
});

// cardSchema.index({ type: 1, value: 1 });

// TODO - pre & post


cardSchema.pre('save', { document: true, query: false }, function(next) { // document middleware - pre - save
  if (this.isModified('user') || this.isModified('is_enabled') || this.isModified('value') || this.isModified('has_validity') || this.isModified('valid_since') || this.isModified('valid_until') /*|| this.isModified('type')*/) {
    this._wasModified = true;
  }
  if (this.isNew) {
    this._wasNew = true;
  }
  next();
});

cardSchema.post('save', { document: true, query: false }, async function(doc, next) { // document middleware - post - save
  if (doc._wasModified) {
    if (doc._wasNew) { // new document
      var userObj = await userService.getOne(false, false, { _id: doc.user });
      userObj.cards.push(doc._id);
      userObj.save();
    } else { // existing document
      if (doc._user != doc.user) { // user has been changed
        var oldUser = await userService.exists({ _id: doc._user });
        if (oldUser) {
          oldUser.cards.pull(doc._id);
          await oldUser.save()
        }
        var newUser = await userService.getOne(false, false, { _id: doc.user });
        newUser.cards.push(doc._id);
        await newUser.save();
      }
      // TODO - handle others
    }

    // TODO - handle other changes (update security groups on both users, push sync to devices, ...)
  }
  next();
});

// TODO - handle delete hooks

// TODO - handle other changes (update security groups on user, push sync to devices, ...)

cardSchema.pre('remove', { document: true, query: false }, async function(next) { // document middleware - pre - remove
  var user = await userService.exists({ _id: this.user });
  if (user) {
    user.cards.pull(this._id);
    await user.save()
  }
  next();
});

// cardSchema.post('remove', { document: true, query: false }, async function(doc, next) { // document middleware - post - remove
//   console.log('POST REMOVE TRIGGER');
//   console.log(doc);
//   next();
// });

export default mongoose.model('card', cardSchema);
