import 'package:flutter/material.dart';

import 'package:sentinel_client/src/pages/unsupportedPage.dart';

class AndroidApp extends StatefulWidget {
  @override
  State<AndroidApp> createState() => _AndroidAppState();
}

class _AndroidAppState extends State<AndroidApp> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      // title: 'Flutter Demo - Android',
      // home: HomePage(title: 'Flutter Demo Home Page - Android'),
      title: 'Sentinel',
      home: UnsupportedPage(),
    );
  }
}
