import 'package:flutter/material.dart';

import 'package:sentinel_client/src/pages/pageWrapper/page_wrapper.windows.dart';
import 'package:sentinel_client/src/pages/authPage/auth_page.windows.dart';
import 'package:sentinel_client/src/pages/loginPage/login_page.windows.dart';
import 'package:sentinel_client/src/pages/homePage/home_page.windows.dart';

class WindowsApp extends StatefulWidget {
  @override
  State<WindowsApp> createState() => _WindowsAppState();
}

class _WindowsAppState extends State<WindowsApp> {
  // void initState() {
  //   super.initState();
  // }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Sentinel',
      // initialRoute: '/auth',
      initialRoute: '/auth',
      routes: {
        '/auth': (context) => WindowsAuthPage(),
        '/': (context) => WindowsPageWrapper(page: WindowsHomePage()),
        '/login': (context) => WindowsLoginPage(),
      },
    );
  }
}
