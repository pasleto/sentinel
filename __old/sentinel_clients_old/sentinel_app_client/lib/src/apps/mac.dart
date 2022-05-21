import 'package:flutter/material.dart';

import 'package:sentinel_client/src/pages/unsupportedPage.dart';

class MacApp extends StatefulWidget {
  @override
  State<MacApp> createState() => _MacAppState();
}

class _MacAppState extends State<MacApp> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      // title: 'Flutter Demo - Mac',
      // home: HomePage(title: 'Flutter Demo Home Page - Mac'),
      title: 'Sentinel',
      home: UnsupportedPage(),
    );
  }
}
