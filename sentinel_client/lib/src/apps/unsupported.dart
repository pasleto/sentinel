import 'package:flutter/material.dart';

import 'package:sentinel_client/src/pages/unsupportedPage.dart';

class UnsupportedApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Sentinel',
      home: UnsupportedPage(),
    );
  }
}
