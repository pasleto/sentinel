import 'package:flutter/material.dart';

// import 'package:sentinel_client/src/pages/unsupportedPage.dart';

import 'package:sentinel_client/src/pages/authPage/auth_page.web.dart';
// import 'package:sentinel_client/src/pages/pageWrapper/page_wrapper.web.dart';
// import 'package:sentinel_client/src/pages/loginPage/login_page.web.dart';
// import 'package:sentinel_client/src/pages/homePage/home_page.web.dart';

class WebApp extends StatefulWidget {
  @override
  State<WebApp> createState() => _WebAppState();
}

class _WebAppState extends State<WebApp> {
  // void initState() {
  //   super.initState();
  // }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Sentinel',
      // home: UnsupportedPage(),
      initialRoute: '/auth',
      routes: {
        '/auth': (context) => WebAuthPage(),
        // '/': (context) => WebPageWrapper(page: WebHomePage()),
        // '/login': (context) => WebLoginPage(),
      },
    );
  }
}
