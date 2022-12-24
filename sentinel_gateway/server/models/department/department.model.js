import mongoose from 'mongoose';
// import autopopulatePlugin from 'mongoose-autopopulate';

const departmentSchema = new mongoose.Schema({ // TODO - add users array
  department_number: { // TODO - maybe possible to add more departments into one 
    type: String,
    reguired: true,
    trim: true,
    unique: true,
    set: function(department_number) {
      this._department_number = this.department_number;
      return department_number;
    },
  },
  name: {
    type: String,
    reguired: true,
    trim: true,
    unique: true,
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
  manager: {
    type: mongoose.Types.ObjectId,
    ref: 'user',
    set: function(manager) {
      this._manager = this.manager;
      return manager;
    },
  },
  is_root: {
    type: Boolean,
    required: true,
    default: false,
    set: function(is_root) {
      this._is_root = this.is_root;
      return is_root;
    },
  },
  can_have_child: {
    type: Boolean,
    required: true,
    default: true,
    set: function(can_have_child) {
      this._can_have_child = this.can_have_child;
      return can_have_child;
    },
  },
  parent: {
    type: mongoose.Types.ObjectId,
    required: function () { return !this.is_root },
    ref: 'department',
    set: function(parent) {
      this._parent = this.parent;
      return parent;
    },
    // autopopulate: true,
  },
  ldap_sync: {
    type: Boolean,
    required: true,
    default: false,
    set: function(ldap_sync) {
      this._ldap_sync = this.ldap_sync;
      return ldap_sync;
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

// departmentSchema.plugin(autopopulatePlugin);

export default mongoose.model('department', departmentSchema);
