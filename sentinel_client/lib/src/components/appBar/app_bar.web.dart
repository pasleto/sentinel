import 'package:flutter/material.dart';

class WebAppBar extends StatelessWidget implements PreferredSizeWidget {
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
    );
  }

  @override
  Size get preferredSize {
    return new Size.fromHeight(30);
  }
}
