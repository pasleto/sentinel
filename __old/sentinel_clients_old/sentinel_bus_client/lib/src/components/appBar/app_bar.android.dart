import 'package:flutter/material.dart';

class AndroidAppBar extends StatefulWidget implements PreferredSizeWidget {
  @override
  _AndroidAppBarState createState() => _AndroidAppBarState();

  @override
  Size get preferredSize {
    return new Size.fromHeight(40);
  }
}

class _AndroidAppBarState extends State<AndroidAppBar> {
  _currentPageRoute() {
    var route = ModalRoute.of(this.context);
    if (route != null) {
      print(route.settings.name); // TODO
      return route.settings.name;
    }
  }

  _handleCancel() {
    // TODO - handle hive store save for actual ride
    print('cancel');
    Navigator.pushNamedAndRemoveUntil(context, '/home', (route) => false); // TODO - auth vs login path
  }

  _handleLogout() {
    // TODO - handle hive store cleanup for actual ride
    print('logout');
    Navigator.pushNamedAndRemoveUntil(context, '/login', (route) => false); // TODO - auth vs login path
  }

  _handleSettings() {
    // TODO
    print('settings');
  }

  Widget _logo() {
    return Padding(
      padding: const EdgeInsets.only(top: 10.0, bottom: 10.0),
      child: Image.asset('assets/logo/Logo_Black_32.png', filterQuality: FilterQuality.high),
    );
  }

  Widget _icon() {
    return Padding(
      padding: const EdgeInsets.only(top: 10.0, bottom: 10.0),
      child: Image.asset('assets/icons/bus_32.png', filterQuality: FilterQuality.high),
    );
  }

  Widget _backButton() {
    return IconButton(onPressed: () => Navigator.of(context).pop(), icon: Icon(Icons.arrow_back_ios_rounded, color: Colors.black), splashRadius: 18.0);
  }

  Widget _cancelButton() {
    return TextButton(onPressed: () => _handleCancel(), child: Text('Cancel', style: TextStyle(color: Colors.black, fontWeight: FontWeight.normal)));
  }

  Widget _logoutButton() {
    return TextButton(onPressed: () => _handleLogout(), child: Text('Logout', style: TextStyle(color: Colors.black, fontWeight: FontWeight.normal)));
  }

  Widget _settingsButton() {
    return IconButton(onPressed: () => _handleSettings(), icon: Icon(Icons.settings, color: Colors.black), splashRadius: 18.0);
  }

  _handleBackgroundColor() {
    var route = _currentPageRoute();
    if (route == '/login' || route == '/auth') return Colors.transparent;
    return null;
  }

  _handleLeftSide() {
    var route = _currentPageRoute();
    if (route == '/home') return _logo();
    if (route == '/ride') return _icon();
    if (route == '/drive-type') return _backButton();
    if (route == '/route-type') return _backButton();
    return Container(width: 0, height: 0);
  }

  _handleCenter() {
    var route = _currentPageRoute();
    if (route == '/route-type') return Text('Choose route', style: TextStyle(color: Colors.black, fontWeight: FontWeight.normal, fontSize: 16));
    if (route == '/ride')
      return Text('Line 1: Source - Destination', style: TextStyle(color: Colors.black, fontWeight: FontWeight.normal, fontSize: 16)); // TODO
    return Container(width: 0, height: 0);
  }

  _handleRightSide() {
    var route = _currentPageRoute();
    if (route == '/home' || route == '/drive-type' || route == '/route-type') return _logoutButton();
    if (route == '/ride') return _cancelButton();
    if (route == '/login') return _settingsButton();
    return Container(width: 0, height: 0);
  }

  @override
  Widget build(BuildContext context) {
    return AppBar(
      automaticallyImplyLeading: false,
      backgroundColor: this._handleBackgroundColor(),
      shadowColor: _handleBackgroundColor(),
      leading: this._handleLeftSide(),
      title: _handleCenter(),
      titleSpacing: 5.0,
      actions: [
        Padding(
          padding: const EdgeInsets.only(right: 10.0, left: 15.0),
          child: this._handleRightSide(),
        ),
      ],
    );
  }
}
