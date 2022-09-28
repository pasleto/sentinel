const departmentInitData = [
  {
    'department_number': '00_sentinel_root_department',
    'name': 'Root',
    'is_root': true,
    'can_have_child': true,
    'description': 'Root Department',
    'ldap_sync': false,
    'is_protected': true
  },
  {
    'department_number': '01_sentinel_unsorted_department',
    'name': 'Unsorted',
    'is_root': true,
    'can_have_child': false,
    'description': 'Unsorted Users',
    'ldap_sync': false,
    'is_protected': true
  },
  {
    'department_number': '02_sentinel_disabled_department',
    'name': 'Disabled',
    'is_root': true,
    'can_have_child': false,
    'description': 'Disabled Users',
    'ldap_sync': false,
    'is_protected': true
  },
  {
    'department_number': '03_sentinel_deleted_department',
    'name': 'Deleted',
    'is_root': true,
    'can_have_child': false,
    'description': 'Deleted Users',
    'ldap_sync': false,
    'is_protected': true
  }
];

export default departmentInitData;
