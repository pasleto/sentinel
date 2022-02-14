import 'package:flutter/material.dart';
import 'package:bitsdojo_window/bitsdojo_window.dart';

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

class WindowButtons extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        MinimizeWindowButton(colors: buttonColors),
        MaximizeWindowButton(colors: buttonColors),
        CloseWindowButton(colors: closeButtonColors),
      ],
    );
  }
}

class WindowsAppBar extends StatelessWidget implements PreferredSizeWidget {
  @override
  Widget build(BuildContext context) {
    return AppBar(
      automaticallyImplyLeading: false,
      titleSpacing: 10.0,
      leading: Builder(
        builder: (context) => IconButton(
          splashColor: Colors.transparent,
          highlightColor: Colors.transparent,
          hoverColor: Colors.transparent,
          focusColor: Colors.transparent,
          icon: Image.asset("assets/icons/drawer_icon.png", filterQuality: FilterQuality.high),
          onPressed: () => Scaffold.of(context).openDrawer(),
        ),
      ),
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
                      child: Image.asset('assets/logo/Logo_Black_32.png', filterQuality: FilterQuality.high, height: 18),
                    ),
                    Padding(
                      padding: const EdgeInsets.only(top: 7.0, bottom: 7.0),
                      child: Image.asset('assets/logo/Logo_Text_Black_16.png', filterQuality: FilterQuality.high, height: 12),
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
        WindowButtons(),
      ],
    );
  }

  @override
  Size get preferredSize {
    return new Size.fromHeight(30);
  }
}
