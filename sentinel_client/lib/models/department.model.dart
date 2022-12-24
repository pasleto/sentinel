import 'package:sentinel_client/models/user.model.dart';

class Department {
  final String id;
  /*final*/ String departmentNumber;
  /*final*/ String name;
  /*final*/ String description;
  final bool isRoot;
  /*final*/ bool canHaveChild;
  /*final*/ bool ldapSync;
  final bool isProtected;
  /*final*/ String? parentId;
  /*final*/ List<User> users;
  /*final*/ List<Department> departments;

  Department(
    this.id,
    this.name,
    this.users,
    this.departmentNumber,
    this.description,
    this.isRoot,
    this.canHaveChild,
    this.ldapSync,
    this.isProtected,
    this.parentId,
    this.departments,
  );

  Department.fromJson(Map<String, dynamic> json)
      : id = json['_id'],
        departmentNumber = json['department_number'],
        name = json['name'],
        description = json['description'],
        isRoot = json['is_root'],
        canHaveChild = json['can_have_child'],
        ldapSync = json['ldap_sync'],
        isProtected = json['is_protected'],
        parentId = json['parent'],
        users = List<User>.from(json['users'].map((model) => User.fromJson(model))),
        departments = List<Department>.from(json['departments'].map((model) => Department.fromJson(model)));

  // ? maybe replace with .every to be able to stop the search
  /// will fill list with all users met condition
  ///
  /// Usage:
  /// ```dart
  /// List<Department> departmentsList = [...];
  /// List searchList = [];
  /// Department.getUsers(departmentsList, 'search_term', searchList);
  /// ```
  static void getUsers(List<Department> list, String search, List out) {
    for (var item in list) {
      for (var user in item.users) {
        if (user.userNumber.toLowerCase().contains(search.toLowerCase()) ||
            user.name.toLowerCase().contains(search.toLowerCase()) ||
            user.surname.toLowerCase().contains(search.toLowerCase()) ||
            user.username.toLowerCase().contains(search.toLowerCase())) {
          out.add(user);
        }
        // ? if strip accents, or search for exact match
        // if (AppUtils.stripAccents(user.userNumber).toLowerCase().contains(AppUtils.stripAccents(search).toLowerCase()) ||
        //     AppUtils.stripAccents(user.name).toLowerCase().contains(AppUtils.stripAccents(search).toLowerCase()) ||
        //     AppUtils.stripAccents(user.surname).toLowerCase().contains(AppUtils.stripAccents(search).toLowerCase()) ||
        //     AppUtils.stripAccents(user.username).toLowerCase().contains(AppUtils.stripAccents(search).toLowerCase())) {
        //   out.add(user);
        // }
      }
      getUsers(item.departments, search, out);
    }
  }

  /// will fill list with only one User object when met condition
  ///
  /// Usage:
  /// ```dart
  /// List<Department> departmentsList = [...];
  /// List searchList = [];
  /// Department.getUserByCard(departmentsList, 'search_term', searchList);
  /// ```
  static void getUserByCard(List<Department> list, String search, List out) {
    bool found = false; // this will stop loops when found
    list.every((item) {
      item.users.every((user) {
        if (user.cards != null) {
          user.cards!.every((card) {
            if (card.value.toLowerCase() == search.toLowerCase()) {
              found = true;
              out.add(user);
              return false;
            }
            return true;
          });
        }
        if (found) return false;
        return true;
      });
      if (found) return false;
      getUserByCard(item.departments, search, out);
      return true;
    });
  }

  // ? maybe replace with .every to be able to stop the search
  /// will fill list with all departments met condition
  ///
  /// Usage:
  /// ```dart
  /// List<Department> departmentsList = [...];
  /// List searchList = [];
  /// Department.getDepartments(departmentsList, 'search_term', searchList);
  /// ```
  static void getDepartments(List<Department> list, String search, List out) {
    for (var item in list) {
      if (!item.isRoot) {
        if (item.name.toLowerCase().contains(search.toLowerCase()) || item.departmentNumber.toLowerCase().contains(search.toLowerCase())) {
          out.add(item);
        }
        // ? if strip accents, or search for exact match
        // if (AppUtils.stripAccents(item.name).toLowerCase().contains(AppUtils.stripAccents(search).toLowerCase()) ||
        //     AppUtils.stripAccents(item.departmentNumber).toLowerCase().contains(AppUtils.stripAccents(search).toLowerCase())) {
        //   out.add(user);
        // }
      }
      getDepartments(item.departments, search, out);
    }
  }

  /// will return one User object searched by user.id if exist, otherwise return null
  ///
  /// Usage:
  /// ```dart
  /// List<Department> departmentsList = [...];
  /// User? usr = Department.getUserById(departmentsList, '<user_id>');
  /// ```
  static User? getUserById(List<Department> list, String searchId) {
    List out = [];

    _getUserById(list, searchId, out);

    if (out.isNotEmpty) {
      return out.first;
    } else {
      return null;
    }
  }

  // private method to search for one user by id
  static void _getUserById(List<Department> list, String searchId, List out) {
    bool found = false; // this will stop loops when found
    list.every((item) {
      item.users.every((user) {
        if (user.id == searchId) {
          found = true;
          out.add(user);
          return false;
        }
        return true;
      });
      if (found) return false;
      _getUserById(item.departments, searchId, out);
      return true;
    });
  }

  /// will return one Department object searched by department.id if exist, otherwise return null
  ///
  /// Usage:
  /// ```dart
  /// List<Department> departmentsList = [...];
  /// Department? dep = Department.getDepartmentById(departmentsList, '<department_id>');
  /// ```
  static Department? getDepartmentById(List<Department> list, String searchId) {
    List out = [];

    _getDepartmentById(list, searchId, out);

    if (out.isNotEmpty) {
      return out.first;
    } else {
      return null;
    }
  }

  // private method to search for one department by id
  static void _getDepartmentById(List<Department> list, String searchId, List out) {
    bool found = false; // this will stop loops when found
    list.every((item) {
      if (item.id == searchId) {
        found = true;
        out.add(item);
        return false;
      }
      if (found) return false;
      _getDepartmentById(item.departments, searchId, out);
      return true;
    });
  }

  /// will fill list with department parents id strings [including itself]
  /// Usage:
  /// ```dart
  /// List<Department> departmentsList = [...];
  /// List<String> parentList = [];
  /// Department.getDepartmentParentList(departmentsList, '<department_id>', parentList);
  /// ```
  static void getDepartmentParentList(List<Department> list, String departmentId, List out) {
    out.add(departmentId);
    Department? dep = getDepartmentById(list, departmentId);
    if (dep != null) {
      if (dep.parentId != null) {
        getDepartmentParentList(list, dep.parentId!, out);
      }
    }
  }

  // ? - get one list of all departments from sub-departments of department
  static void getAllSubDepartments(List<Department> list, List out) {
    for (var item in list) {
      out.addAll(item.departments);
      getAllSubDepartments(item.departments, out);
    }
    // out.addAll(list);
    // for (var item in list) {
    //   getAllSubDepartments(item.departments, out);
    // }
  }

  // ? - get one list of all users from sub-departments of department
  static void getAllSubUsers(List<Department> list, List out) {
    for (var item in list) {
      out.addAll(item.users);
      getAllSubUsers(item.departments, out);
    }
  }
}
