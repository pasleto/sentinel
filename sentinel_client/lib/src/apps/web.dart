import 'package:flutter/material.dart';

import 'package:sentinel_client/src/pages/unsupportedPage.dart';

class WebApp extends StatefulWidget {
  @override
  State<WebApp> createState() => _WebAppState();
}

class _WebAppState extends State<WebApp> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      // title: 'Flutter Demo - Web',
      // home: HomePage(title: 'Flutter Demo Home Page - Web'),
      title: 'Sentinel',
      home: UnsupportedPage(),
    );
  }
}
