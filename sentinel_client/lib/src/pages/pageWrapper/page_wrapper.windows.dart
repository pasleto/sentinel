import 'package:flutter/material.dart';

import 'package:sentinel_client/src/components/appBar/app_bar.windows.dart';
import 'package:sentinel_client/src/components/appDrawer/app_drawer.windows.dart';

class WindowsPageWrapper extends StatefulWidget {
  WindowsPageWrapper({Key? key, required this.page}) : super(key: key);

  final Widget page;

  @override
  _WindowsPageWrapperState createState() => _WindowsPageWrapperState();
}

class _WindowsPageWrapperState extends State<WindowsPageWrapper> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: WindowsAppBar(),
      drawer: WindowsAppDrawer(),
      body: widget.page,
    );
  }
}
