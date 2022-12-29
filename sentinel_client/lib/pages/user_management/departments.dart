import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/models/department.model.dart';
import 'package:sentinel_client/models/user.model.dart';
import 'package:sentinel_client/services/socket_service.dart';
import 'package:sentinel_client/widgets/page.dart';
import 'package:sentinel_client/widgets/toasts.dart';

import 'package:visibility_detector/visibility_detector.dart';

// TODO - create global provider for blocking navigation state -> if something is marked as unsaved changes, it will show dialog when try to navigate elsewhere

// user card
// 04C2B401, B -> Tomáš Pásler
// A -> Jan Novák
// F4EE851D -> Test 2

// TODO - when rfid reader scanned on search TextBox -> Error: Assertion failed: org-dartlang-sdk:///flutter_web_sdk/lib/_engine/engine/keyboard_binding.dart:355:16 -> result != null "Invalid modifier location: 4, 1"
// TODO - all numpad numbers
// TODO - only vscode pause this error, otherwise typed correctly

class UserManagementDepartmentsPage extends StatefulWidget {
  const UserManagementDepartmentsPage({super.key});

  @override
  State<UserManagementDepartmentsPage> createState() => _UserManagementDepartmentsPageState();
}

class _UserManagementDepartmentsPageState extends State<UserManagementDepartmentsPage> with PageMixin {
  bool _pageInScope = false;

  bool _initialized = false;
  List<Department> _departments = [];
  List<String> _expandList = [];
  String _selectedElementId = '';
  dynamic _selectedElement;
  List<Department> _selectedElementSumDepartments = []; // TODO - reset ?
  List<User> _selectedElementSumUsers = []; // TODO - reset ?
  bool _canChangeItem = true;

  final TextEditingController _controllerSearch = TextEditingController();
  final FocusNode _focusNodeSearch = FocusNode();
  String _searchMode = 'user';
  String? _searchLastValue;
  List? _searchLastList;
  int _searchLastIndex = 0;
  bool _searchRunning = false; // TODO
  bool _fetchingFailed = false;

  bool _isElementChangeMode = false;
  bool _isElementChangeSyncing = false;

  // userBox
  String _userBottomNavigationTab = 'cards';
  final TextEditingController _userControllerUserNumber = TextEditingController();
  final TextEditingController _userControllerName = TextEditingController();
  final TextEditingController _userControllerSurname = TextEditingController();
  final TextEditingController _userControllerUsername = TextEditingController();
  final TextEditingController _userControllerEmail = TextEditingController();
  final TextEditingController _userControllerTelephone = TextEditingController();
  final TextEditingController _userControllerMobile = TextEditingController();
  final TextEditingController _userControllerJobtitle = TextEditingController();

  // departmentBox
  final TextEditingController _departmentControllerName = TextEditingController();
  final TextEditingController _departmentControllerDepartmentNumber = TextEditingController();
  final TextEditingController _departmentControllerDescription = TextEditingController();
  bool _departmentControllerCanHaveChild = false;
  bool _departmentControllerLdapSync = false;

  void initUserBoxControllers() {
    // TODO - clear ?
    _userControllerUserNumber.text = _selectedElement.userNumber;
    _userControllerName.text = _selectedElement.name;
    _userControllerSurname.text = _selectedElement.surname;
    _userControllerUsername.text = _selectedElement.username;
    _userControllerEmail.text = _selectedElement.email ?? ''; // ?
    _userControllerTelephone.text = _selectedElement.telephone ?? ''; // ?
    _userControllerMobile.text = _selectedElement.mobile ?? ''; // ?
    _userControllerJobtitle.text = _selectedElement.jobTitle ?? ''; // ?
  }

  void calculateDepartmentCounts() {
    List<Department> sumDepartments = [..._selectedElement.departments];
    Department.getAllSubDepartments(_selectedElement.departments, sumDepartments);
    List<User> sumUsers = [..._selectedElement.users];
    Department.getAllSubUsers(_selectedElement.departments, sumUsers);
    setState(() {
      _selectedElementSumDepartments = sumDepartments;
      _selectedElementSumUsers = sumUsers;
    });
  }

  void initDepartmentBoxControllers() {
    // TODO - clear ?
    _departmentControllerName.text = _selectedElement.name;
    _departmentControllerDepartmentNumber.text = _selectedElement.departmentNumber;
    _departmentControllerDescription.text = _selectedElement.description;
    _departmentControllerCanHaveChild = _selectedElement.canHaveChild;
    _departmentControllerLdapSync = _selectedElement.ldapSync;
  }

  void departmentEdit() {
    // TODO

    print('Department edit request');

    // TODO

    setState(() {
      _isElementChangeMode = true;
      _canChangeItem = false;
    });
  }

  void saveDepartmentEdit() {
    // TODO

    setState(() {
      _isElementChangeSyncing = true;
    });

    // TODO

    Future.delayed(const Duration(seconds: 5), () {
      // TODO - test only -> make function to push data to backend + update element in array, then unblock states
      _selectedElement.name = 'Test${widget.hashCode}';
      setState(() {
        _isElementChangeMode = false;
        _isElementChangeSyncing = false;
        _canChangeItem = true;
      });
    });
  }

  void discardDepartmentEdit() {
    // TODO

    print('Department discard changes request');

    // TODO

    initDepartmentBoxControllers();
    setState(() {
      _isElementChangeMode = false;
      _canChangeItem = true;
    });
  }

  void userEdit() {
    // TODO

    print('User edit request');

    // TODO

    setState(() {
      _isElementChangeMode = true;
      _canChangeItem = false;
    });
  }

  void saveUserEdit() {
    // TODO

    setState(() {
      _isElementChangeSyncing = true;
    });

    // TODO

    Future.delayed(const Duration(seconds: 5), () {
      // TODO - test only -> make function to push data to backend + update element in array, then unblock states
      _selectedElement.name = 'Test${widget.hashCode}';
      setState(() {
        _isElementChangeMode = false;
        _isElementChangeSyncing = false;
        _canChangeItem = true;
      });
    });
  }

  void discardUserEdit() {
    // TODO

    print('User discard changes request');

    // TODO

    initUserBoxControllers();
    setState(() {
      _isElementChangeMode = false;
      _canChangeItem = true;
    });
  }

  void _resetSearch() {
    setState(() {
      _searchLastValue = null;
      _searchLastList = null;
      _searchLastIndex = 0;
      _searchRunning = false;
    });
  }

  void _resetSearchWithClear() {
    _controllerSearch.clear();
    _resetSearch();
  }

  void _setSearchMode(mode) {
    _resetSearchWithClear();
    _focusNodeSearch.requestFocus();
    setState(() {
      _searchMode = mode;
    });
  }

  // TODO - handle if can change item
  void _newSearch() {
    setState(() {
      _searchRunning = true;
    });

    List searchList = [];
    switch (_searchMode) {
      case 'user':
        Department.getUsers(_departments, _controllerSearch.text, searchList);
        break;
      case 'card':
        Department.getUserByCard(_departments, _controllerSearch.text, searchList);
        break;
      case 'department':
        Department.getDepartments(_departments, _controllerSearch.text, searchList);
        break;
    }

    if (searchList.isNotEmpty) {
      AppToasts.genericNotification(
        context: context,
        messageText: () {
          switch (_searchMode) {
            case 'user':
              return searchList.length > 1 ? '${searchList.length} users found' : '${searchList.length} user found';
            case 'card':
              return 'User found';
            case 'department':
              return searchList.length > 1 ? '${searchList.length} departments found' : '${searchList.length} department found';
          }
        }(),
        closeButton: false,
        duration: const Duration(seconds: 3),
      );

      List<String> parentList = [];
      switch (_searchMode) {
        case 'user':
        case 'card':
          Department.getDepartmentParentList(_departments, searchList[_searchLastIndex].departmentId, parentList);
          break;
        case 'department':
          Department.getDepartmentParentList(_departments, searchList[_searchLastIndex].parentId, parentList); // excluding self
          // Department.getDepartmentParentList(_departments, searchList[_searchLastIndex].id, parentList); // including self
          break;
      }

      List<String> expand = _expandList;
      for (var parent in parentList) {
        if (!expand.contains(parent)) {
          expand.add(parent);
        }
      }

      setState(() {
        _searchLastValue = _controllerSearch.text;
        _searchLastList = searchList;
        _expandList = expand;
        _selectedElement = searchList[_searchLastIndex];
        _selectedElementId = searchList[_searchLastIndex].id;
        _searchRunning = false;
      });

      switch (_searchMode) {
        case 'user':
        case 'card':
          setState(() {
            _userBottomNavigationTab = 'cards';
          });
          initUserBoxControllers();
          break;
        case 'department':
          calculateDepartmentCounts();
          initDepartmentBoxControllers();
          break;
      }

      if (searchList.length > 1) {
        _focusNodeSearch.requestFocus();
      }
    } else {
      AppToasts.genericNotification(
        context: context,
        messageText: () {
          switch (_searchMode) {
            case 'user':
              return 'No user found';
            case 'card':
              return 'No user found';
            case 'department':
              return 'No department found';
          }
        }(),
        closeButton: false,
        duration: const Duration(seconds: 3),
      );
      _resetSearch();
    }
  }

  // TODO - handle if can change item
  void _existingSearch() {
    if (_searchLastIndex == _searchLastList!.length - 1) {
      _resetSearch();
      _newSearch();
    } else {
      // setState(() {
      //   _searchRunning = true;
      // });

      int newIndex = _searchLastIndex + 1;
      if (newIndex <= _searchLastList!.length - 1) {
        List<String> parentList = [];
        switch (_searchMode) {
          case 'user':
            // case 'card':
            Department.getDepartmentParentList(_departments, _searchLastList![newIndex].departmentId, parentList);
            break;
          case 'department':
            Department.getDepartmentParentList(_departments, _searchLastList![newIndex].parentId, parentList); // excluding self
            // Department.getDepartmentParentList(_departments, _searchLastList![newIndex].id, parentList); // including self
            break;
        }

        List<String> newExpand = _expandList;
        for (var parent in parentList) {
          if (!newExpand.contains(parent)) {
            newExpand.add(parent);
          }
        }

        setState(() {
          _searchLastIndex = newIndex;
          _expandList = newExpand;
          _selectedElement = _searchLastList![newIndex];
          _selectedElementId = _searchLastList![newIndex].id;
          _searchRunning = false;
        });

        switch (_searchMode) {
          case 'user':
            setState(() {
              _userBottomNavigationTab = 'cards';
            });
            initUserBoxControllers();
            break;
          case 'department':
            calculateDepartmentCounts();
            initDepartmentBoxControllers();
            break;
        }

        if (newIndex < _searchLastList!.length - 1) {
          _focusNodeSearch.requestFocus();
        }
      }
    }
  }

  // TODO - handle if can change item
  void _search() {
    switch (_searchMode) {
      case 'user':
        if (_controllerSearch.text.isNotEmpty) {
          if (_searchLastValue == _controllerSearch.text) {
            _existingSearch();
          } else {
            _resetSearch();
            _newSearch();
          }
        } else {
          _resetSearch();
        }
        break;
      case 'card':
        if (_controllerSearch.text.isNotEmpty) {
          _resetSearch();
          _newSearch();
        } else {
          _resetSearch();
        }
        break;
      case 'department':
        if (_controllerSearch.text.isNotEmpty) {
          if (_searchLastValue == _controllerSearch.text) {
            _existingSearch();
          } else {
            _resetSearch();
            _newSearch();
          }
        } else {
          _resetSearch();
        }
        break;
    }
  }

  // TODO - handle if can change item
  void unsavedChangesDialog(dynamic item) {
    // ? - maybe pass onPressCallback function instead of item and replace setState with onPressCallback call
    if (_isElementChangeSyncing) {
      AppToasts.warningNotification(
        context: context,
        messageText: 'Running sync operation',
        duration: const Duration(seconds: 3),
        closeButton: false,
      );
      setState(() {
        _selectedElementId = _selectedElementId;
        _selectedElement = _selectedElement;
      });
    } else {
      showDialog<String>(
        context: context,
        barrierDismissible: false,
        builder: (context) => ContentDialog(
          title: const Text('Unsaved changes'),
          content: const Text("Discard changes?"),
          actions: [
            Button(
              child: const Text('Discard'),
              onPressed: () {
                setState(() {
                  _selectedElementId = item.id;
                  _selectedElement = item;
                  _canChangeItem = true;
                  _isElementChangeMode = false;
                });
                Navigator.pop(context);
              },
            ),
            FilledButton(
              child: const Text('Cancel'),
              onPressed: () {
                setState(() {
                  _selectedElementId = _selectedElementId;
                  _selectedElement = _selectedElement;
                });
                Navigator.pop(context);
              },
            ),
          ],
        ),
      );
    }
  }

  List<TreeViewItem> generateList(Department department) {
    List<TreeViewItem> output = [];
    for (var element in department.departments) {
      output.add(
        TreeViewItem(
          content: Text('${element.name} [${element.departmentNumber}]', maxLines: 2, overflow: TextOverflow.ellipsis),
          value: element.id,
          expanded: _expandList.contains(element.id),
          selected: _selectedElementId == element.id,
          leading: const Icon(FluentIcons.external_user, size: 18),
          children: generateList(element),
          onInvoked: (item, reason) async {
            if (_selectedElementId != element.id) {
              if (reason == TreeViewItemInvokeReason.pressed) {
                if (_canChangeItem) {
                  setState(() {
                    _selectedElementId = element.id;
                    _selectedElement = element;
                  });
                  calculateDepartmentCounts();
                  initDepartmentBoxControllers();
                } else {
                  unsavedChangesDialog(element); // TODO
                }
              }
            }
          },
        ),
      );
    }
    for (var element in department.users) {
      output.add(
        TreeViewItem(
          content: Text('${element.name} ${element.surname} [${element.userNumber}]', maxLines: 2, overflow: TextOverflow.ellipsis),
          value: element.id,
          collapsable: false,
          expanded: false,
          selected: _selectedElementId == element.id,
          leading: const Icon(FluentIcons.contact, size: 14),
          onInvoked: (item, reason) async {
            if (_selectedElementId != element.id) {
              if (reason == TreeViewItemInvokeReason.pressed) {
                if (_canChangeItem) {
                  setState(() {
                    _selectedElementId = element.id;
                    _selectedElement = element;
                    _userBottomNavigationTab = 'cards';
                  });
                  initUserBoxControllers(); // TODO
                } else {
                  unsavedChangesDialog(element); // TODO
                }
              }
            }
          },
        ),
      );
    }
    return output;
  }

  @override
  void setState(fn) {
    if (mounted) {
      super.setState(fn);
    }
  }

  @override
  void initState() {
    super.initState();

    print('DEPARTMENTS initState');

    _controllerSearch.addListener(() {
      if (_controllerSearch.text.isEmpty) {
        _resetSearch();
      }
    });

    // SocketService.socket.emitWithAck('department-get-list', {}, ack: (response) {
    //   if (response['status'] == 'OK') {
    //     Iterable incoming = response['data']['list'];
    //     setState(() {
    //       _departments = List<Department>.from(incoming.map((model) => Department.fromJson(model)));
    //       _initialized = true;
    //       _fetchingFailed = false;
    //     });

    //     // TODO - when refreshed data come, refresh data for selectedElement if there is any
    //     // TODO - if the selectedElement is in dirty state (currently being modified) -> show notification with action to either continue in edit, or refresh with new data
    //     // Future.delayed(const Duration(seconds: 10), () { // ? test only
    //     //   setState(() {
    //     //     _initialized = false;
    //     //   });
    //     //   Future.delayed(const Duration(seconds: 5), () {
    //     //     SocketService.socket.emitWithAck('department-get-list', {}, ack: (response) {
    //     //       if (response['status'] == 'OK') {
    //     //         Iterable incoming = response['data']['list'];
    //     //         setState(() {
    //     //           _departments = List<Department>.from(incoming.map((model) => Department.fromJson(model)));
    //     //           _initialized = true;
    //     //           _fetchingFailed = false;
    //     //         });
    //     //       } else {
    //     //         var msg = response['data']['message'];
    //     //         AppToasts.errorNotification(
    //     //           context: context,
    //     //           titleText: 'Fetching Data Failed',
    //     //           messageText: msg ?? 'Generic Failure',
    //     //           duration: const Duration(seconds: 5),
    //     //         );
    //     //         setState(() {
    //     //           _initialized = true;
    //     //           _fetchingFailed = false;
    //     //         });
    //     //       }
    //     //     });
    //     //   });
    //     // });
    //   } else {
    //     var msg = response['data']['message'];
    //     AppToasts.errorNotification(
    //       context: context,
    //       titleText: 'Fetching Data Failed',
    //       messageText: msg ?? 'Generic Failure',
    //       duration: const Duration(seconds: 5),
    //     );
    //     setState(() {
    //       _initialized = true;
    //       _departments = [];
    //       _fetchingFailed = true;
    //     });
    //   }
    // });
  }

  @override
  void dispose() {
    _controllerSearch.dispose();
    _focusNodeSearch.dispose();

    // TODO
    _userControllerUserNumber.dispose();
    _userControllerName.dispose();
    _userControllerSurname.dispose();
    _userControllerUsername.dispose();
    _userControllerEmail.dispose();
    _userControllerTelephone.dispose();
    _userControllerMobile.dispose();
    _userControllerJobtitle.dispose();

    _departmentControllerName.dispose();
    _departmentControllerDepartmentNumber.dispose();
    _departmentControllerDescription.dispose();
    // TODO

    print('DEPARTMENTS dispose');

    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return VisibilityDetector(
      key: const Key('user-management-departments'),
      onVisibilityChanged: (visibilityInfo) {
        if (visibilityInfo.visibleFraction == 1) {
          if (!_pageInScope) {
            setState(() => _pageInScope = !_pageInScope);

            print('user-management-departments loading');

            SocketService.socket.emitWithAck('department-get-list', {}, ack: (response) {
              if (response['status'] == 'OK') {
                Iterable incoming = response['data']['list'];
                setState(() {
                  _departments = List<Department>.from(incoming.map((model) => Department.fromJson(model)));
                  _initialized = true;
                  _fetchingFailed = false;
                });

                // TODO - when refreshed data come, refresh data for selectedElement if there is any
                // TODO - if the selectedElement is in dirty state (currently being modified) -> show notification with action to either continue in edit, or refresh with new data
                // Future.delayed(const Duration(seconds: 10), () { // ? test only
                //   setState(() {
                //     _initialized = false;
                //   });
                //   Future.delayed(const Duration(seconds: 5), () {
                //     SocketService.socket.emitWithAck('department-get-list', {}, ack: (response) {
                //       if (response['status'] == 'OK') {
                //         Iterable incoming = response['data']['list'];
                //         setState(() {
                //           _departments = List<Department>.from(incoming.map((model) => Department.fromJson(model)));
                //           _initialized = true;
                //           _fetchingFailed = false;
                //         });
                //       } else {
                //         var msg = response['data']['message'];
                //         AppToasts.errorNotification(
                //           context: context,
                //           titleText: 'Fetching Data Failed',
                //           messageText: msg ?? 'Generic Failure',
                //           duration: const Duration(seconds: 5),
                //         );
                //         setState(() {
                //           _initialized = true;
                //           _fetchingFailed = false;
                //         });
                //       }
                //     });
                //   });
                // });
              } else {
                var msg = response['data']['message'];
                AppToasts.errorNotification(
                  context: context,
                  titleText: 'Fetching Data Failed',
                  messageText: msg ?? 'Generic Failure',
                  duration: const Duration(seconds: 5),
                );
                setState(() {
                  _initialized = true;
                  _departments = [];
                  _fetchingFailed = true;
                });
              }
            });
          }
        }
        if (visibilityInfo.visibleFraction == 0) {
          if (_pageInScope) {
            setState(() => _pageInScope = !_pageInScope);

            print('user-management-departments unloading');
          }
        }
      },
      child: ScaffoldPage(
        header: PageHeader(
          title: const Text('User Management', style: TextStyle(fontSize: 18.0, fontWeight: FontWeight.w600)),
          // commandBar: CommandBarCard(
          //   padding: const EdgeInsets.symmetric(horizontal: 6.0, vertical: 2.0),
          //   child: CommandBar(
          commandBar: Padding(
            padding: const EdgeInsets.symmetric(vertical: 3.0),
            child: CommandBar(
              mainAxisAlignment: MainAxisAlignment.end,
              overflowBehavior: CommandBarOverflowBehavior.dynamicOverflow,
              isCompact: true,
              primaryItems: [
                if (!_initialized)
                  CommandBarBuilderItem(
                    builder: (context, mode, w) => Tooltip(
                      displayHorizontally: false,
                      useMousePosition: false,
                      style: const TooltipThemeData(
                        showDuration: Duration(milliseconds: 500),
                        waitDuration: Duration(milliseconds: 500),
                        preferBelow: true,
                      ),
                      message: "Fetching data",
                      child: w,
                    ),
                    wrappedItem: const CommandBarButton(
                      icon: Padding(
                        padding: EdgeInsets.only(right: 12.0),
                        child: SizedBox(
                          height: 16,
                          width: 16,
                          child: ProgressRing(strokeWidth: 3.5),
                        ),
                      ),
                      onPressed: null,
                    ),
                  ),
                if (_fetchingFailed)
                  CommandBarBuilderItem(
                    builder: (context, mode, w) => Tooltip(
                      displayHorizontally: false,
                      useMousePosition: false,
                      style: const TooltipThemeData(
                        showDuration: Duration(milliseconds: 500),
                        waitDuration: Duration(milliseconds: 500),
                        preferBelow: true,
                      ),
                      message: "Fetching data failed",
                      child: w,
                    ),
                    wrappedItem: const CommandBarButton(
                      icon: Padding(
                        padding: EdgeInsets.only(right: 12.0),
                        child: SizedBox(
                          height: 16,
                          width: 16,
                          child: Icon(FluentIcons.sync_error, color: Colors.warningPrimaryColor),
                        ),
                      ),
                      onPressed: null,
                    ),
                  ),
                CommandBarBuilderItem(
                  builder: (context, mode, w) => Tooltip(
                    displayHorizontally: false,
                    useMousePosition: false,
                    style: const TooltipThemeData(
                      showDuration: Duration(milliseconds: 500),
                      waitDuration: Duration(milliseconds: 500),
                      preferBelow: true,
                    ),
                    message: "Add department",
                    child: w,
                  ),
                  wrappedItem: CommandBarButton(
                    icon: const Icon(FluentIcons.add),
                    label: const Text('New'),
                    onPressed: () {
                      // TODO
                      print('New department request');
                    },
                  ),
                ),
                // const CommandBarSeparator(),
                CommandBarBuilderItem(
                  builder: (context, mode, w) => Padding(
                    padding: const EdgeInsets.symmetric(vertical: 0.0, horizontal: 3.0),
                    child: w,
                  ),
                  wrappedItem: const CommandBarSeparator(),
                ),
                CommandBarBuilderItem(
                  builder: (context, mode, w) => Tooltip(
                    displayHorizontally: false,
                    useMousePosition: false,
                    style: const TooltipThemeData(
                      showDuration: Duration(milliseconds: 500),
                      waitDuration: Duration(milliseconds: 500),
                      preferBelow: true,
                    ),
                    message: "Add user",
                    child: w,
                  ),
                  wrappedItem: CommandBarButton(
                    icon: const Icon(FluentIcons.add_friend),
                    label: const Text('New'),
                    onPressed: () {
                      // TODO
                      print('New user request');
                    },
                  ),
                ),
              ],
            ),
          ),
        ),
        content: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          mainAxisAlignment: MainAxisAlignment.start,
          mainAxisSize: MainAxisSize.max,
          children: [
            Container(width: double.infinity, height: 1, color: const Color.fromARGB(100, 175, 175, 175)),
            Expanded(
              child: Row(
                crossAxisAlignment: CrossAxisAlignment.start,
                mainAxisAlignment: MainAxisAlignment.start,
                mainAxisSize: MainAxisSize.max,
                children: [
                  Expanded(
                    flex: 1,
                    child: Column(
                      children: [
                        Padding(
                          padding: const EdgeInsets.all(6.0),
                          child: Row(
                            crossAxisAlignment: CrossAxisAlignment.center,
                            mainAxisAlignment: MainAxisAlignment.center,
                            mainAxisSize: MainAxisSize.max,
                            children: [
                              Expanded(
                                child: TextBox(
                                  maxLines: 1,
                                  minLines: 1,
                                  minHeight: 35,
                                  readOnly: _departments.isEmpty,
                                  enabled: _departments.isNotEmpty,
                                  textAlignVertical: TextAlignVertical.center,
                                  controller: _controllerSearch,
                                  autocorrect: false,
                                  keyboardType: () {
                                    switch (_searchMode) {
                                      case 'user':
                                        return TextInputType.text;
                                      case 'card':
                                        return TextInputType.none;
                                      case 'department':
                                        return TextInputType.text;
                                    }
                                  }(),
                                  focusNode: _focusNodeSearch,
                                  onSubmitted: (value) => _search(),
                                  placeholder: () {
                                    switch (_searchMode) {
                                      case 'user':
                                        return ' Name, Surname, Username, Number';
                                      case 'card':
                                        return ' Value';
                                      case 'department':
                                        return ' Name, Number';
                                    }
                                  }(),
                                  placeholderStyle: const TextStyle(
                                    fontSize: 12,
                                    fontWeight: FontWeight.w300,
                                    fontStyle: FontStyle.italic,
                                  ),
                                  style: const TextStyle(height: 1),
                                  suffix: GestureDetector(
                                    onTap: _resetSearchWithClear,
                                    child: const Padding(
                                      padding: EdgeInsets.only(right: 6.0, bottom: 1.5),
                                      child: Icon(FluentIcons.status_circle_error_x, size: 20),
                                    ),
                                  ),
                                  suffixMode: OverlayVisibilityMode.editing,
                                ),
                              ),
                              Tooltip(
                                displayHorizontally: false,
                                useMousePosition: false,
                                style: const TooltipThemeData(
                                  showDuration: Duration(milliseconds: 500),
                                  waitDuration: Duration(milliseconds: 500),
                                  preferBelow: true,
                                ),
                                message: "Search",
                                child: Padding(
                                  padding: const EdgeInsets.only(left: 4.0),
                                  child: SizedBox(
                                    height: 34,
                                    child: Button(
                                      onPressed: _departments.isEmpty ? null : _search, // TODO - when search is running | possible to stop search ?
                                      child: const Padding(
                                        padding: EdgeInsets.only(bottom: 1.0),
                                        child: Icon(FluentIcons.search, size: 14.0), // TODO - when search is running -> spinner
                                      ),
                                    ),
                                  ),
                                ),
                              ),
                              Tooltip(
                                displayHorizontally: false,
                                useMousePosition: false,
                                style: const TooltipThemeData(
                                  showDuration: Duration(milliseconds: 500),
                                  waitDuration: Duration(milliseconds: 500),
                                  preferBelow: true,
                                ),
                                message: "Search mode",
                                child: Padding(
                                  padding: const EdgeInsets.only(left: 4.0),
                                  child: SizedBox(
                                    height: 34,
                                    child: DropDownButton(
                                      placement: FlyoutPlacement.end,
                                      closeAfterClick: true,
                                      disabled: _departments.isEmpty, // TODO - when search is running
                                      leading: () {
                                        switch (_searchMode) {
                                          case 'user':
                                            return const Padding(
                                              padding: EdgeInsets.only(left: 1.0, right: 2.0, bottom: 1.5, top: 0.5),
                                              child: SizedBox(
                                                width: 18,
                                                height: 18,
                                                child: Icon(FluentIcons.contact, size: 15),
                                              ),
                                            );
                                          case 'card':
                                            return const Padding(
                                              padding: EdgeInsets.only(left: 1.0, right: 2.0, bottom: 1.5, top: 0.5),
                                              child: SizedBox(
                                                width: 18,
                                                height: 18,
                                                child: Icon(FluentIcons.i_d_badge, size: 18),
                                              ),
                                            );
                                          case 'department':
                                            return const Padding(
                                              padding: EdgeInsets.only(left: 1.0, right: 2.0, bottom: 1.5, top: 0.5),
                                              child: SizedBox(
                                                width: 18,
                                                height: 18,
                                                child: Icon(FluentIcons.external_user, size: 18),
                                              ),
                                            );
                                        }
                                      }(),
                                      trailing: const Icon(FluentIcons.chevron_down, size: 8),
                                      items: [
                                        MenuFlyoutItem(
                                          leading: const Icon(FluentIcons.contact),
                                          selected: _searchMode == 'user',
                                          text: const Text('Person'),
                                          onPressed: () => _setSearchMode('user'),
                                        ),
                                        MenuFlyoutItem(
                                          leading: const Icon(FluentIcons.i_d_badge),
                                          selected: _searchMode == 'card',
                                          text: const Text('User Card'),
                                          onPressed: () => _setSearchMode('card'),
                                        ),
                                        MenuFlyoutItem(
                                          leading: const Icon(FluentIcons.external_user),
                                          selected: _searchMode == 'department',
                                          text: const Text('Department'),
                                          onPressed: () => _setSearchMode('department'),
                                        ),
                                      ],
                                    ),
                                  ),
                                ),
                              ),
                            ],
                          ),
                        ),
                        Container(width: double.infinity, height: 1, color: const Color.fromARGB(100, 175, 175, 175)),
                        Expanded(
                          child: Padding(
                            padding: const EdgeInsets.all(4.0),
                            child: TreeView(
                              selectionMode: TreeViewSelectionMode.single,
                              shrinkWrap: true,
                              onItemInvoked: null,
                              onSelectionChanged: null,
                              onItemExpandToggle: (item, getsExpanded) async {
                                if (getsExpanded) {
                                  _expandList.add(item.value);
                                } else {
                                  _expandList.remove(item.value);
                                }
                              },
                              items: _departments.isEmpty
                                  ? [
                                      TreeViewItem(
                                        content: () {
                                          if (_fetchingFailed) {
                                            return const Text('Data fetch failed!', maxLines: 2, overflow: TextOverflow.ellipsis);
                                          } else {
                                            return const Text('Fetching data ...', maxLines: 2, overflow: TextOverflow.ellipsis);
                                          }
                                        }(),
                                        expanded: false,
                                        collapsable: false,
                                        leading: () {
                                          if (_fetchingFailed) {
                                            return const Icon(FluentIcons.sync_error, size: 14, color: Colors.warningPrimaryColor);
                                          } else {
                                            return const SizedBox(
                                              width: 6,
                                              height: 20,
                                              child: Padding(padding: EdgeInsets.all(4.0), child: ProgressRing(strokeWidth: 2.0)),
                                            );
                                          }
                                        }(),
                                      ),
                                    ]
                                  : List<TreeViewItem>.generate(_departments.length, (index) {
                                      assert(index >= 0);
                                      Department root = _departments[index];
                                      return TreeViewItem(
                                        content: Text(root.name, maxLines: 2, overflow: TextOverflow.ellipsis),
                                        value: root.id,
                                        expanded: _expandList.contains(root.id),
                                        selected: _selectedElementId == root.id,
                                        leading: const Icon(FluentIcons.archive, size: 16),
                                        children: generateList(root),
                                        onInvoked: (item, reason) async {
                                          if (reason == TreeViewItemInvokeReason.pressed) {
                                            setState(() {
                                              _selectedElementId = _selectedElementId;
                                              _selectedElement = _selectedElement;
                                            });
                                          }
                                        },
                                      );
                                    }),
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                  Expanded(flex: 0, child: Container(width: 1, color: const Color.fromARGB(100, 175, 175, 175))),
                  Expanded(
                    flex: 2,
                    child: () {
                      switch (_selectedElement.runtimeType) {
                        case Department:
                          if (_selectedElement.isRoot) {
                            return const SizedBox.shrink();
                          } else {
                            return departmentBox();
                            // return DepartmentBox(selectedDepartment: _selectedElement);
                          }
                        case User:
                          return userBox();
                        // return UserBox(selectedUser: _selectedElement);
                        default:
                          return const SizedBox.shrink();
                      }
                    }(),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget departmentBox() {
    return ScaffoldPage(
      padding: const EdgeInsets.only(top: 12.0),
      header: PageHeader(
        padding: 12.0,
        title: SizedBox(
          height: 34,
          child: Row(
            children: [
              const Padding(
                padding: EdgeInsets.only(right: 12.0),
                child: Icon(FluentIcons.external_user, size: 25),
              ),
              Text(
                '${_selectedElement.departmentNumber}',
                maxLines: 1,
                overflow: TextOverflow.ellipsis,
                style: const TextStyle(fontSize: 20, fontWeight: FontWeight.w500),
              ),
            ],
          ),
        ),
        // commandBar: CommandBarCard(
        //   // padding: const EdgeInsets.all(0),
        //   child: CommandBar(
        commandBar: Padding(
          padding: const EdgeInsets.symmetric(vertical: 5.0),
          child: CommandBar(
            mainAxisAlignment: MainAxisAlignment.end,
            overflowBehavior: CommandBarOverflowBehavior.noWrap,
            isCompact: false,
            primaryItems: _isElementChangeMode
                ? _isElementChangeSyncing
                    ? [
                        CommandBarBuilderItem(
                          builder: (context, mode, w) => Tooltip(
                            displayHorizontally: false,
                            useMousePosition: false,
                            style: const TooltipThemeData(
                              showDuration: Duration(milliseconds: 500),
                              waitDuration: Duration(milliseconds: 500),
                              preferBelow: true,
                            ),
                            message: "Syncing data",
                            child: w,
                          ),
                          wrappedItem: const CommandBarButton(
                            icon: SizedBox(
                              height: 16,
                              width: 16,
                              child: ProgressRing(strokeWidth: 3.5),
                            ),
                            label: Text('Syncing changes'),
                            onPressed: null,
                          ),
                        ),
                      ]
                    : [
                        CommandBarBuilderItem(
                          builder: (context, mode, w) => Tooltip(
                            displayHorizontally: false,
                            useMousePosition: false,
                            style: const TooltipThemeData(
                              showDuration: Duration(milliseconds: 500),
                              waitDuration: Duration(milliseconds: 500),
                              preferBelow: true,
                            ),
                            message: "Save changes",
                            child: w,
                          ),
                          wrappedItem: CommandBarButton(
                            icon: const Icon(FluentIcons.save, color: Colors.successPrimaryColor),
                            label: const Text('Save'),
                            onPressed: saveDepartmentEdit,
                          ),
                        ),
                        CommandBarBuilderItem(
                          builder: (context, mode, w) => Padding(
                            padding: const EdgeInsets.symmetric(vertical: 0.0, horizontal: 3.0),
                            child: SizedBox(
                              height: 32,
                              child: w,
                            ),
                          ),
                          wrappedItem: const CommandBarSeparator(),
                        ),
                        CommandBarBuilderItem(
                          builder: (context, mode, w) => Tooltip(
                            displayHorizontally: false,
                            useMousePosition: false,
                            style: const TooltipThemeData(
                              showDuration: Duration(milliseconds: 500),
                              waitDuration: Duration(milliseconds: 500),
                              preferBelow: true,
                            ),
                            message: "Discard changes",
                            child: w,
                          ),
                          wrappedItem: CommandBarButton(
                            icon: const Icon(FluentIcons.delete, color: Colors.warningPrimaryColor),
                            label: const Text('Discard'),
                            onPressed: discardDepartmentEdit,
                          ),
                        ),
                      ]
                : [
                    CommandBarBuilderItem(
                      builder: (context, mode, w) => Tooltip(
                        displayHorizontally: false,
                        useMousePosition: false,
                        style: const TooltipThemeData(
                          showDuration: Duration(milliseconds: 500),
                          waitDuration: Duration(milliseconds: 500),
                          preferBelow: true,
                        ),
                        message: "Edit",
                        child: w,
                      ),
                      wrappedItem: CommandBarButton(
                        icon: const Icon(FluentIcons.edit),
                        label: const Text('Edit'),
                        onPressed: departmentEdit,
                      ),
                    ),
                  ],
          ),
        ),
      ),
      content: Column(
        children: [
          Padding(
            padding: const EdgeInsets.symmetric(vertical: 6.0, horizontal: 6.0),
            child: Row(
              children: [
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: boxTextField(_departmentControllerName, 'Name', TextInputType.text, null), // ? required, validator
                  ),
                ),
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: boxTextField(_departmentControllerDepartmentNumber, 'Department Number', TextInputType.text, null), // ? required, validator
                  ),
                ),
              ],
            ),
          ),
          Padding(
            padding: const EdgeInsets.symmetric(vertical: 6.0, horizontal: 6.0),
            child: Row(
              children: [
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: boxTextField(_departmentControllerDescription, 'Description', TextInputType.text, null), // ? required, validator
                  ),
                ),
              ],
            ),
          ),
          Padding(
            padding: const EdgeInsets.symmetric(vertical: 6.0, horizontal: 6.0),
            child: Row(
              children: [
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: InfoLabel(
                      label: 'Sub-Departments',
                      child: ToggleSwitch(
                        checked: _departmentControllerCanHaveChild,
                        onChanged: _isElementChangeMode
                            ? _isElementChangeSyncing
                                ? null
                                : (v) => setState(
                                    () => _departmentControllerCanHaveChild = v) // TODO - if disabling and department already contains some subdepartments
                            : null,
                        content: Flyout(
                          openMode: FlyoutOpenMode.press,
                          content: (context) {
                            return const FlyoutContent(
                              child: Text('Whether this department can contain sub-departments'),
                            );
                          },
                          child: const Padding(
                            padding: EdgeInsets.only(top: 0.5, left: 12.0),
                            child: Icon(FluentIcons.info, size: 14),
                          ),
                        ),
                      ),
                    ),
                  ),
                ),
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: InfoLabel(
                      label: 'Synchronization',
                      child: ToggleSwitch(
                        checked: _departmentControllerLdapSync,
                        onChanged: _isElementChangeMode
                            ? _isElementChangeSyncing
                                ? null
                                : (v) =>
                                    setState(() => _departmentControllerLdapSync = v) // TODO - if disabling, any problems ? | if enabling, perform any action ?
                            : null,
                        content: Flyout(
                          openMode: FlyoutOpenMode.press,
                          content: (context) {
                            return const FlyoutContent(
                              child: Text('Whether this department should be synchronized from LDAP based on Department Number value'),
                            );
                          },
                          child: const Padding(
                            padding: EdgeInsets.only(top: 0.5, left: 12.0),
                            child: Icon(FluentIcons.info, size: 14),
                          ),
                        ),
                      ),
                    ),
                  ),
                ),
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: InfoLabel(
                      label: 'Sub-Departments',
                      child: Padding(
                        padding: const EdgeInsets.symmetric(horizontal: 6.0, vertical: 6.0),
                        child: Row(
                          children: [
                            Text('${_selectedElement.departments.length} / ${_selectedElementSumDepartments.length}'),
                            Flyout(
                              openMode: FlyoutOpenMode.press,
                              content: (context) {
                                return const FlyoutContent(
                                  child: Text('Direct sub-departments of this department / Total count of sub-department tree'),
                                );
                              },
                              child: const Padding(
                                padding: EdgeInsets.only(top: 0.5, left: 12.0),
                                child: Icon(FluentIcons.info, size: 14),
                              ),
                            ),
                          ],
                        ),
                      ),
                    ),
                  ),
                ),
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: InfoLabel(
                      label: 'Users',
                      child: Padding(
                        padding: const EdgeInsets.symmetric(horizontal: 6.0, vertical: 6.0),
                        child: Row(
                          children: [
                            Text('${_selectedElement.users.length} / ${_selectedElementSumUsers.length}'),
                            Flyout(
                              openMode: FlyoutOpenMode.press,
                              content: (context) {
                                return const FlyoutContent(
                                  child: Text('Direct users in this department / Total count of user in sub-department tree'),
                                );
                              },
                              child: const Padding(
                                padding: EdgeInsets.only(top: 0.5, left: 12.0),
                                child: Icon(FluentIcons.info, size: 14),
                              ),
                            ),
                          ],
                        ),
                      ),
                    ),
                  ),
                ),
              ],
            ),
          ),
          Padding(padding: const EdgeInsets.only(top: 6.0), child: Container(height: 1, color: const Color.fromARGB(100, 175, 175, 175))),
          Expanded(
            child: Row(
              children: [
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.only(bottom: 12.0, top: 12.0, left: 12.0, right: 6.0),
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        const Padding(
                          padding: EdgeInsets.only(bottom: 12.0),
                          child: Text('Users (Whole Sub-Tree):'),
                        ),
                        Expanded(
                          child: ListView.builder(
                            itemCount: _selectedElementSumUsers.length,
                            itemBuilder: (context, index) {
                              final user = _selectedElementSumUsers[index];
                              return ListTile.selectable(
                                onPressed: null,
                                onSelectionChange: null,
                                selectionMode: ListTileSelectionMode.none,
                                selected: false,
                                title: Text('${user.name} ${user.surname}'),
                                subtitle: Text('[${user.userNumber}]'),
                                // trailing: , // TODO
                              );
                            },
                          ),
                        ),
                      ],
                    ),
                  ),
                ),
                Padding(padding: const EdgeInsets.symmetric(horizontal: 6.0), child: Container(width: 1, color: const Color.fromARGB(100, 175, 175, 175))),
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.only(bottom: 12.0, top: 12.0, left: 6.0, right: 12.0),
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        if (_departmentControllerCanHaveChild)
                          const Padding(
                            padding: EdgeInsets.only(bottom: 12.0),
                            child: Text('Departments (Whole Sub-Tree):'),
                          ),
                        if (_departmentControllerCanHaveChild)
                          Expanded(
                            child: ListView.builder(
                              itemCount: _selectedElementSumDepartments.length,
                              itemBuilder: (context, index) {
                                final department = _selectedElementSumDepartments[index];
                                return ListTile.selectable(
                                  onPressed: null,
                                  onSelectionChange: null,
                                  selectionMode: ListTileSelectionMode.none,
                                  selected: false,
                                  title: Text('${department.name}'),
                                  subtitle: Text('[${department.departmentNumber}]'),
                                  // trailing: , // TODO
                                );
                              },
                            ),
                          ),
                      ],
                    ),
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }

  Widget userBox() {
    return ScaffoldPage(
      padding: const EdgeInsets.only(top: 12.0),
      header: PageHeader(
        padding: 12.0,
        title: SizedBox(
          height: 34,
          child: Row(
            children: [
              const Padding(
                padding: EdgeInsets.only(right: 12.0),
                child: Icon(FluentIcons.contact, size: 19),
              ),
              Text(
                '${_selectedElement.userNumber}',
                maxLines: 1,
                overflow: TextOverflow.ellipsis,
                style: const TextStyle(fontSize: 20, fontWeight: FontWeight.w500),
              ),
            ],
          ),
        ),
        // commandBar: CommandBarCard(
        //   padding: const EdgeInsets.all(0),
        //   child: CommandBar(
        commandBar: Padding(
          padding: const EdgeInsets.symmetric(vertical: 5.0),
          child: CommandBar(
            mainAxisAlignment: MainAxisAlignment.end,
            overflowBehavior: CommandBarOverflowBehavior.noWrap,
            isCompact: false,
            primaryItems: _isElementChangeMode
                ? _isElementChangeSyncing
                    ? [
                        CommandBarBuilderItem(
                          builder: (context, mode, w) => Tooltip(
                            displayHorizontally: false,
                            useMousePosition: false,
                            style: const TooltipThemeData(
                              showDuration: Duration(milliseconds: 500),
                              waitDuration: Duration(milliseconds: 500),
                              preferBelow: true,
                            ),
                            message: "Syncing data",
                            child: w,
                          ),
                          wrappedItem: const CommandBarButton(
                            icon: SizedBox(
                              height: 16,
                              width: 16,
                              child: ProgressRing(strokeWidth: 3.5),
                            ),
                            label: Text('Syncing changes'),
                            onPressed: null,
                          ),
                        ),
                      ]
                    : [
                        CommandBarBuilderItem(
                          builder: (context, mode, w) => Tooltip(
                            displayHorizontally: false,
                            useMousePosition: false,
                            style: const TooltipThemeData(
                              showDuration: Duration(milliseconds: 500),
                              waitDuration: Duration(milliseconds: 500),
                              preferBelow: true,
                            ),
                            message: "Save changes",
                            child: w,
                          ),
                          wrappedItem: CommandBarButton(
                            icon: const Icon(FluentIcons.save, color: Colors.successPrimaryColor),
                            label: const Text('Save'),
                            onPressed: saveUserEdit,
                          ),
                        ),
                        CommandBarBuilderItem(
                          builder: (context, mode, w) => Padding(
                            padding: const EdgeInsets.symmetric(vertical: 0.0, horizontal: 3.0),
                            child: SizedBox(
                              height: 32,
                              child: w,
                            ),
                          ),
                          wrappedItem: const CommandBarSeparator(),
                        ),
                        CommandBarBuilderItem(
                          builder: (context, mode, w) => Tooltip(
                            displayHorizontally: false,
                            useMousePosition: false,
                            style: const TooltipThemeData(
                              showDuration: Duration(milliseconds: 500),
                              waitDuration: Duration(milliseconds: 500),
                              preferBelow: true,
                            ),
                            message: "Discard changes",
                            child: w,
                          ),
                          wrappedItem: CommandBarButton(
                            icon: const Icon(FluentIcons.delete, color: Colors.warningPrimaryColor),
                            label: const Text('Discard'),
                            onPressed: discardUserEdit,
                          ),
                        ),
                      ]
                : [
                    CommandBarBuilderItem(
                      builder: (context, mode, w) => Tooltip(
                        displayHorizontally: false,
                        useMousePosition: false,
                        style: const TooltipThemeData(
                          showDuration: Duration(milliseconds: 500),
                          waitDuration: Duration(milliseconds: 500),
                          preferBelow: true,
                        ),
                        message: "Edit",
                        child: w,
                      ),
                      wrappedItem: CommandBarButton(
                        icon: const Icon(FluentIcons.edit),
                        label: const Text('Edit'),
                        onPressed: userEdit,
                      ),
                    ),
                  ],
          ),
        ),
        // ),
      ),
      // TODO
      // content: Padding(
      //   padding: const EdgeInsets.only(left: 12.0, right: 12.0),
      //   child: Column(
      content: Column(
        children: [
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 12.0),
            child: Row(
              children: [
                Expanded(
                  child: Column(
                    children: [
                      Padding(
                        padding: const EdgeInsets.symmetric(vertical: 6.0),
                        child: Row(
                          children: [
                            Expanded(
                              child: Padding(
                                padding: const EdgeInsets.only(right: 12.0),
                                child: boxTextField(_userControllerName, 'First Name', TextInputType.name, null), // ? required
                              ),
                            ),
                            Expanded(
                              child: Padding(
                                padding: const EdgeInsets.only(right: 12.0),
                                child: boxTextField(_userControllerSurname, 'Last Name', TextInputType.name, null), // ? required
                              ),
                            ),
                          ],
                        ),
                      ),
                      Padding(
                        padding: const EdgeInsets.symmetric(vertical: 6.0),
                        child: Row(
                          children: [
                            Expanded(
                              child: Padding(
                                padding: const EdgeInsets.only(right: 12.0),
                                child: boxTextField(_userControllerUserNumber, 'User Number', TextInputType.text, null), // ? required, validator
                              ),
                            ),
                            Expanded(
                              child: Padding(
                                padding: const EdgeInsets.only(right: 12.0),
                                child: boxTextField(_userControllerUsername, 'Username', TextInputType.text, null),
                              ),
                            ),
                            Expanded(
                              child: Padding(
                                padding: const EdgeInsets.only(right: 12.0),
                                child: boxTextField(_userControllerJobtitle, 'Job Title', TextInputType.text, null),
                              ),
                            ),
                          ],
                        ),
                      ),
                      Padding(
                        padding: const EdgeInsets.symmetric(vertical: 6.0),
                        child: Row(
                          children: [
                            Expanded(
                              flex: 4,
                              child: Padding(
                                padding: const EdgeInsets.only(right: 12.0),
                                child: boxTextField(_userControllerEmail, 'Email', TextInputType.emailAddress, null),
                              ),
                            ),
                            Expanded(
                              flex: 2,
                              child: Padding(
                                padding: const EdgeInsets.only(right: 12.0),
                                child: boxTextField(_userControllerTelephone, 'Telephone', TextInputType.phone, null),
                              ),
                            ),
                            Expanded(
                              flex: 2,
                              child: Padding(
                                padding: const EdgeInsets.only(right: 12.0),
                                child: boxTextField(_userControllerMobile, 'Mobile', TextInputType.phone, null),
                              ),
                            ),
                          ],
                        ),
                      ),
                    ],
                  ),
                ),
                SizedBox(
                  width: 182,
                  height: 182,
                  child: Image.memory(
                    _selectedElement.getImageData(),
                    gaplessPlayback: true,
                    fit: BoxFit.fill,
                  ),
                ),
              ],
            ),
          ),
          Padding(
            padding: const EdgeInsets.symmetric(vertical: 6.0, horizontal: 6.0),
            child: Row(
              children: [
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: boxTextField(TextEditingController(), 'AAA', TextInputType.text, null), // TODO
                  ),
                ),
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: boxTextField(TextEditingController(), 'BBB', TextInputType.text, null), // TODO
                  ),
                ),
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: boxTextField(TextEditingController(), 'CCC', TextInputType.text, null), // TODO
                  ),
                ),
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 6.0),
                    child: boxTextField(TextEditingController(), 'DDD', TextInputType.text, null), // TODO
                  ),
                ),
              ],
            ),
          ),
          Padding(padding: const EdgeInsets.symmetric(vertical: 6.0), child: Container(height: 1, color: const Color.fromARGB(100, 175, 175, 175))),
          Expanded(
            child: Padding(
              padding: const EdgeInsets.only(top: 6.0, bottom: 12.0, left: 12.0, right: 12.0),
              child: () {
                switch (_userBottomNavigationTab) {
                  case 'cards':
                    // return Text('cards');
                    // TODO
                    return Container(
                      color: Colors.blue.light,
                      width: double.maxFinite,
                      height: double.maxFinite,
                    );
                  case 'permissions':
                    // return Text('permissions');
                    // TODO
                    return Container(
                      color: Colors.green.light,
                      width: double.maxFinite,
                      height: double.maxFinite,
                    );
                  default:
                    return const SizedBox.shrink();
                }
              }(),
            ),
          ),
        ],
      ),
      // ),
      bottomBar: Column(
        children: [
          Container(height: 1, color: const Color.fromARGB(100, 175, 175, 175)),
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              Expanded(
                child: SizedBox(
                  height: 40,
                  child: Tooltip(
                    displayHorizontally: false,
                    useMousePosition: false,
                    style: const TooltipThemeData(
                      showDuration: Duration(milliseconds: 500),
                      waitDuration: Duration(milliseconds: 500),
                      preferBelow: true,
                    ),
                    message: "User Cards",
                    child: IconButton(
                      style: ButtonStyle(shape: ButtonState.all(RoundedRectangleBorder(borderRadius: BorderRadius.circular(0.0)))),
                      icon: () {
                        if (_userBottomNavigationTab == 'cards') {
                          return Icon(FluentIcons.i_d_badge, size: 16, color: FluentTheme.of(context).accentColor);
                        } else {
                          return const Icon(FluentIcons.i_d_badge, size: 16);
                        }
                      }(),
                      onPressed: () {
                        setState(() {
                          _userBottomNavigationTab = 'cards';
                        });
                      },
                    ),
                  ),
                ),
              ),
              Container(height: 40, width: 1, color: const Color.fromARGB(100, 175, 175, 175)),
              Expanded(
                child: SizedBox(
                  height: 40,
                  child: Tooltip(
                    displayHorizontally: false,
                    useMousePosition: false,
                    style: const TooltipThemeData(
                      showDuration: Duration(milliseconds: 500),
                      waitDuration: Duration(milliseconds: 500),
                      preferBelow: true,
                    ),
                    message: "User Permissions",
                    child: IconButton(
                      style: ButtonStyle(shape: ButtonState.all(RoundedRectangleBorder(borderRadius: BorderRadius.circular(0.0)))),
                      icon: () {
                        if (_userBottomNavigationTab == 'permissions') {
                          return Icon(FluentIcons.permissions, size: 16, color: FluentTheme.of(context).accentColor);
                        } else {
                          return const Icon(FluentIcons.permissions, size: 16);
                        }
                      }(),
                      onPressed: () {
                        setState(() {
                          _userBottomNavigationTab = 'permissions';
                        });
                      },
                    ),
                  ),
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }

  TextFormBox boxTextField(TextEditingController fieldController, String headerText, TextInputType inputType, String? Function(String?)? valueValidator) {
    return TextFormBox(
      maxLines: 1,
      minLines: 1,
      textAlignVertical: TextAlignVertical.center,
      enabled: _isElementChangeMode
          ? _isElementChangeSyncing
              ? false
              : true
          : false,
      controller: fieldController,
      header: headerText,
      headerStyle: const TextStyle(overflow: TextOverflow.ellipsis),
      // suffix: , // TODO - icon with tooltip -> error description
      errorHighlightColor: Colors.warningPrimaryColor,
      keyboardType: inputType,
      validator: valueValidator,
    );
  }
}

// ? separated widget ?
// class UserBox extends StatefulWidget {
//   const UserBox({super.key});

//   @override
//   State<UserBox> createState() => _UserBoxState();
// }

// class _UserBoxState extends State<UserBox> {
//   @override
//   Widget build(BuildContext context) {
//      return const SizedBox.shrink();
//   }
// }

// class DepartmentBox extends StatefulWidget {
//   const DepartmentBox({super.key});

//   @override
//   State<DepartmentBox> createState() => _DepartmentBoxState();
// }

// class _DepartmentBoxState extends State<DepartmentBox> {
//   @override
//   Widget build(BuildContext context) {
//      return const SizedBox.shrink();
//   }
// }