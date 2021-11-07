import 'package:flutter/material.dart';

import 'package:sentinel_client/src/pages/unsupportedPage.dart';

class LinuxApp extends StatefulWidget {
  @override
  State<LinuxApp> createState() => _LinuxAppState();
}

class _LinuxAppState extends State<LinuxApp> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      // title: 'Flutter Demo - Linux',
      // home: HomePage(title: 'Flutter Demo Home Page - Linux'),
      title: 'Sentinel',
      home: UnsupportedPage(),
    );
  }
}
