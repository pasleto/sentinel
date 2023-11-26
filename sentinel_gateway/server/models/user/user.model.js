import mongoose from 'mongoose';
import _ from 'underscore';
// import mongooseLeanVirtuals from 'mongoose-lean-virtuals';
import utils from '../../utils/util.js';
import { departmentService, cardService } from '../../controllers/mongo.controller.js';

const userSchema = new mongoose.Schema({ // TODO
  user_number: {
    type: String,
    required: true,
    trim: true,
    unique: true,
    set: function(user_number) {
      this._user_number = this.user_number;
      return user_number;
    },
  },
  name: {
    type: String,
    required: true,
    trim: true,
    set: function(name) {
      this._name = this.name;
      return name;
    },
  },
  surname: {
    type: String,
    required: true,
    trim: true,
    set: function(surname) { // https://coderwall.com/p/xe2m9a/expose-previous-values-in-mongoose
      this._surname = this.surname;
      return surname;
    },
  },
  username: {
    type: String,
    required: function () { return this.login_enabled },
    trim: true,
    set: function(username) {
      this._username = this.username;
      return username;
    },
  },
  email: {
    type: String,
    trim: true,
    set: function(email) {
      this._email = this.email;
      return email;
    },
  },
  mobile: {
    type: String,
    trim: true,
    set: function(mobile) {
      this._mobile = this.mobile;
      return mobile;
    },
  },
  telephone: {
    type: String,
    trim: true,
    set: function(telephone) {
      this._telephone = this.telephone;
      return telephone;
    },
  },
  image: { // default - 96px windows default user image
    type: String,
    trim: true,
    default: 'iVBORw0KGgoAAAANSUhEUgAAAGAAAABgCAMAAADVRocKAAAAnFBMVEXn5+c9PT3q6ura2to/Pz/g4ODV1dVUVFRDQ0Pi4uJKSkrm5ubHx8fAwMCjo6Pk5OSxsbFycnJfX19PT09NTU1GRkbQ0NCCgoJ+fn6oqKiXl5eRkZFjY2NRUVFBQUE6OjrX19fLy8vCwsK9vb24uLienp6bm5t6enp5eXlra2tmZmZcXFzc3Nytra2goKCLi4uEhIRpaWmGhoZvb2+MNFbWAAACz0lEQVRo3u1Y2XLiMBC0JB/I9wG+zX0FAiHJ///bzpgtagEHV1l62ZT6gQfJNS31aDQtNAUFBQUFBQWFXwdKOTMBjFOqyQcNFv6utprGqnf+IpBOESwvjU7+Qm8uy0Bu/HmVkDsk1VymPIvv6+rtJo4b+7qL74U8mRYHAhgdylXKWLoqDyMCOCxkxTffCCAumIZrhh9WxDjyZsqJb2SoyD79dyzdo2aZISUB4QTi54zeDbIcGCahjDQwFOjTpA+05ieKxGRsADI6XtOn8fUY8i5hC0GGYvPnCd5OBOIldgStcQPPW4DcHOfCCnlQV/G8kzqGyvOoKMEJUrA3Oo/vHpJwEibwdTL7cLqmnI8Z0X0pBBXtnKqkEVyCzvN1kUKAZRB15yCSUQjUhXqy0q6p1IIKdIUrjdWETIuuOiimhNTid4Xzcb1zuu8oOF+ioCsbpPafJ3xIjr2iEtpBTiALHn2scAuGcxkNgS4b7I8uvc89dtFmKaUtO1tswceQ01t4Hh6xSW8dST3zS0dHkXkGbWF4mY1d9MvQJIFlyEDGUXlar09lNCYYP2OaNLyXGBNVmUxQL2Qr32U6rzQiD4hSKi88O8fTR4JpfGaSKBwvuobXbauOotqy9StF5Mnx1YVFEMnO90xmGMz0/F1CEFYhwWPz7aSNVbmc0ttLxK1a1smWC6+/jT/KXe2+kjU3H7UMgaD+BcYfb4xn42VsxshQCFXz9Uqzwu6mH7aTQsaFRbj+8KfpEPcQiRT0eQr6b5wfBdxAHqZngZdH3N75Pb0iNgcTbHTQ2KWvLAGkQd8MvuLwCVBpL1Hh4+FdoBsnLn3tapLhnZmWsLod7yn0HXxUDiPge4LWsN9akj0fVgQ3/9/7fhhWCinIa5m9R9mCRKXD7IpNZjXrt5YzYg+yL9Q9Jklu9PuyJDkOs8Ac/3xyeu9bBp9xTUFBQUFBQUHhf8cfdJYm2hAn4e0AAAAASUVORK5CYII=',
    set: function(image) {
      this._image = this.image;
      return image;
    },
  },
  department: {
    type: mongoose.Types.ObjectId,
    ref: 'department',
    required: function () { return !this.is_protected }, // no department for system users
    set: function(department) {
      this._department = this.department;
      return department;
    },
  },
  manager: { // ?
    type: mongoose.Types.ObjectId,
    ref: 'user',
    set: function(manager) {
      this._manager = this.manager;
      return manager;
    },
  },
  is_admin: {
    type: Boolean,
    required: true,
    default: false,
    set: function(is_admin) {
      this._is_admin = this.is_admin;
      return is_admin;
    },
  },
  login_enabled: {
    type: Boolean,
    required: true,
    default: function () { return this.is_admin },
    set: function(login_enabled) {
      this._login_enabled = this.login_enabled;
      return login_enabled;
    },
  },
  cards : [{ // TODO - in card model, implement hooks for create, delete, ... to modify this field on card instance change
    type: mongoose.Types.ObjectId,
    ref: 'card',
  }],

  // permissions: {
  //   type: String, // TODO
  // },

  // application_permissions // TODO - list and scope of permissions to applications when user can use card to logon

  is_ldap_user: { // TODO - handle if changed from true to false (delete sid, ... + request password if login enabled) and vise versa (fetch everything from ldap)
    type: Boolean,
    required: true,
    default: false,
    set: function(is_ldap_user) {
      this._is_ldap_user = this.is_ldap_user;
      return is_ldap_user;
    },
  },
  ldap_sid: {
    type: String,
    required: function () { return this.is_ldap_user },
    trim: true,
    set: function(ldap_sid) {
      this._ldap_sid = this.ldap_sid;
      return ldap_sid;
    },
  },
  job_title: {
    type: String,
    trim: true,
    set: function(job_title) {
      this._job_title = this.job_title;
      return job_title;
    },
  },
  password: {
    type: String,
    required: function () { return (this.login_enabled && !this.is_ldap_user) },
    minLength: 8,
    trim: true,
    set: function(password) {
      this._password = this.password;
      return password;
    },
  },
  is_protected: {
    type: Boolean,
    required: true,
    default: false,
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

userSchema.index({ email: 1 }, { unique: true, partialFilterExpression: { email: { $type: "string" } } }); // unique only of set
userSchema.index({ username: 1 }, { unique: true, partialFilterExpression: { username: { $type: "string" } } });
userSchema.index({ ldap_sid: 1 }, { unique: true, partialFilterExpression: { ldap_sid: { $type: "string" } } });

// userSchema.virtual('fullname').get(function() {
//   return `${this.name} ${this.surname}`;
// }); // ?

// https://mongoosejs.com/docs/middleware.html
// https://mongoosejs.com/docs/documents.html
// https://stackoverflow.com/questions/65463458/how-mongoose-middleware-works-and-what-is-next

// document middlewares - validate,save,remove,updateOne,deleteOne,init
// model middlewares - insertMany
// query middlewares - count,countDocuments,deleteMany,deleteOne,find,findOne,findOneAndDelete,findOneAndRemove,findOneAndReplace,findOneAndUpdate,remove,replaceOne,update,updateOne,updateMany

// https://javascripttricks.com/mongoose-middleware-the-javascript-7d23a96bfcbf

userSchema.pre('validate', { document: true, query: false }, async function(next){
  if (!this.department) {
    var unsortedDepartment = await departmentService.getOne({ department_number: '01_sentinel_unsorted_department' });
    this.department = unsortedDepartment._id;
  }
  next();
});

userSchema.pre('save', { document: true, query: false }, function(next) { // document middleware - pre - save


  // if (this.isNew && xx)

  // console.log('old: ' + this._surname);
  // console.log('new: ' + this.surname);
  if (this.isModified('password') && this.password) { // TODO - add length check, throw error
    this.password = utils.generatePasswordHash(this.password); // hash password if set
  }
  if (this.isModified('email') && this.email) {
    this.email = this.email.toLowerCase(); // make email lower case if set
  }
  this.updatedAt = Date.now();
  next();
  // console.log("%s has changed their surname to %s", this._surname, this.surname);
});

// userSchema.post('save', { document: true, query: false }, function(doc) { // document middleware - post - save
//   // console.log('User Document Saved!'); // TODO
// });

// userSchema.pre('updateOne', { document: true, query: false }, function() { // document middleware - pre - updateOne
//   console.log('Updating User Document!'); // TODO
// });

// userSchema.post('updateOne', { document: true, query: false }, function(doc) { // document middleware - post - updateOne
//   console.log('User Document Updated!'); // TODO
// });

// userSchema.pre('remove', { document: true, query: false }, function() { // document middleware - pre - remove
//   console.log('Removing User Document!'); // TODO
// });

// userSchema.post('remove', { document: true, query: false }, function(doc) { // document middleware - post - remove
//   console.log('User Document Removed!'); // TODO
// });

// userSchema.pre('deleteOne', { document: true, query: false }, function() { // document middleware - pre - remove
//   console.log('Removing User Document!'); // TODO
// });

// userSchema.post('deleteOne', { document: true, query: false }, function(doc) { // document middleware - post - remove
//   console.log('User Document Removed!'); // TODO
// });

// userSchema.plugin(mongooseLeanVirtuals);

export default mongoose.model('user', userSchema);
