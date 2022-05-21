import 'package:flutter/material.dart';

import 'package:sentinel_client/src/pages/unsupportedPage.dart';

class IosApp extends StatefulWidget {
  @override
  State<IosApp> createState() => _IosAppState();
}

class _IosAppState extends State<IosApp> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      // title: 'Flutter Demo - iOS',
      // home: HomePage(title: 'Flutter Demo Home Page - iOS'),
      title: 'Sentinel',
      home: UnsupportedPage(),
    );
  }
}
