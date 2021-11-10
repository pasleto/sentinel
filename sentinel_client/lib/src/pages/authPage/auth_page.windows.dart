import 'package:flutter/material.dart';
import 'package:bitsdojo_window/bitsdojo_window.dart';

import 'package:sentinel_client/src/helpers/device/device_info.windows.dart';

class WindowsAuthPage extends StatefulWidget {
  WindowsAuthPage({Key? key}) : super(key: key);

  @override
  _WindowsAuthPageState createState() => _WindowsAuthPageState();
}

final buttonColors = WindowButtonColors(
  iconNormal: Colors.black,
  mouseOver: Colors.black12,
  mouseDown: Colors.black26,
  iconMouseOver: Colors.white,
  iconMouseDown: Colors.white,
);

final closeButtonColors = WindowButtonColors(
  iconNormal: Colors.black,
  mouseOver: Colors.redAccent[700],
  mouseDown: Colors.red[900],
  iconMouseOver: Colors.white,
  iconMouseDown: Colors.white,
);

class _WindowsAuthAppBar extends StatelessWidget implements PreferredSizeWidget {
  @override
  Widget build(BuildContext context) {
    return AppBar(
      automaticallyImplyLeading: false,
      titleSpacing: 10.0,
      title: Align(
        alignment: Alignment.centerLeft,
        child: WindowTitleBarBox(
          child: Row(
            children: [
              MoveWindow(
                child: Row(
                  children: [
                    Padding(
                      padding: const EdgeInsets.only(top: 7.0, bottom: 7.0, right: 10.0),
                      child: Image.asset('assets/logo/logo_32.png', filterQuality: FilterQuality.high),
                    ),
                    Padding(
                      padding: const EdgeInsets.only(top: 7.0, bottom: 7.0),
                      child: Image.asset('assets/logo/title_32.png', filterQuality: FilterQuality.high),
                    ),
                  ],
                ),
              ),
              Expanded(child: MoveWindow()),
            ],
          ),
        ),
      ),
      actions: [
        Row(children: [
          MinimizeWindowButton(colors: buttonColors),
          MaximizeWindowButton(colors: buttonColors),
          CloseWindowButton(colors: closeButtonColors),
        ]),
      ],
    );
  }

  @override
  Size get preferredSize {
    return new Size.fromHeight(30);
  }
}

class _WindowsAuthPageState extends State<WindowsAuthPage> {
  void initState() {
    super.initState();
    print('UserName: ' + getWindowsUserName());
    print('UserDomain: ' + getWindowsDomainName());
    print('UserDnsDomain: ' + getWindowsDnsDomainName());
    print('ComputerName: ' + getWindowsComputerName());
    print('ComputerOS: ' + getWindowsComputerSystem());
    print('UserAppData: ' + getWindowsUserAppData());
    print('UserLocalAppData: ' + getWindowsUserLocalAppData());
    print('UserProfile: ' + getWindowsUserProfile());
    print('DomainLogonName: ' + getWindowsDomainLogonName());
    print('DomainUserName: ' + getWindowsDomainUserName());
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: _WindowsAuthAppBar(),
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.max,
          crossAxisAlignment: CrossAxisAlignment.center,
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Expanded(
              child: Container(
                alignment: Alignment.center,
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.center,
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    Padding(
                      padding: const EdgeInsets.all(16.0),
                      child: Image.asset('assets/logo/logo_256.png', width: 96, height: 96, filterQuality: FilterQuality.high),
                    ),
                    Padding(
                      padding: const EdgeInsets.all(16.0),
                      child: Container(child: LinearProgressIndicator(minHeight: 4), height: 4, width: 196),
                    ),
                  ],
                ),
              ),
            ),
            Container(
              height: 45,
              alignment: Alignment.center,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.center,
                mainAxisAlignment: MainAxisAlignment.start,
                children: [
                  Text('Sentinel', style: TextStyle(fontWeight: FontWeight.w600, fontSize: 13)),
                  Text('Copyright © 2021 Tomáš Pásler', style: TextStyle(fontWeight: FontWeight.w500, fontStyle: FontStyle.italic, fontSize: 12)),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
