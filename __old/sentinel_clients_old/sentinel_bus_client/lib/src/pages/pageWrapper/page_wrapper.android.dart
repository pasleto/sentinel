import 'package:flutter/material.dart';

import 'package:sentinel_bus_client/src/components/appBar/app_bar.android.dart';

class AndroidPageWrapper extends StatefulWidget {
  AndroidPageWrapper({Key? key, required this.page}) : super(key: key);

  final Widget page;

  @override
  _AndroidPageWrapperState createState() => _AndroidPageWrapperState();
}

class _AndroidPageWrapperState extends State<AndroidPageWrapper> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AndroidAppBar(),
      body: widget.page,
    );
  }
}
