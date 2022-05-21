import 'package:flutter/material.dart';
import 'package:wakelock/wakelock.dart';
import 'package:kiosk_mode/kiosk_mode.dart';
import 'package:flutter/services.dart';

import 'package:sentinel_bus_client/src/pages/authPage/auth_page.android.dart';
import 'package:sentinel_bus_client/src/pages/homePage/home_page.android.dart';
import 'package:sentinel_bus_client/src/pages/routeTypePage/route_type_page.android.dart';
import 'package:sentinel_bus_client/src/pages/loginPage/login_page.android.dart';
import 'package:sentinel_bus_client/src/pages/pageWrapper/page_wrapper.android.dart';
import 'package:sentinel_bus_client/src/pages/driveTypePage/drive_type_page.android.dart';
import 'package:sentinel_bus_client/src/pages/ridePage/ride_page.android.dart';

class AndroidApp extends StatefulWidget {
  @override
  _AndroidAppState createState() => _AndroidAppState();
}

class _AndroidAppState extends State<AndroidApp> {
  void initState() {
    super.initState();
    SystemChrome.setEnabledSystemUIMode(SystemUiMode.manual, overlays: []);
    // startKioskMode();
    Wakelock.enable();
  }

  @override
  void dispose() {
    Wakelock.disable();
    // stopKioskMode();
    SystemChrome.setEnabledSystemUIMode(SystemUiMode.manual, overlays: [SystemUiOverlay.top, SystemUiOverlay.bottom]);
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Sentinel Bus',
      // initialRoute: '/login',
      initialRoute: '/home',
      routes: {
        '/login': (context) => AndroidPageWrapper(page: AndroidLoginPage()),
        '/auth': (context) => AndroidPageWrapper(page: AndroidAuthPage()),
        '/home': (context) => AndroidPageWrapper(page: AndroidHomePage()),
        '/drive-type': (context) => AndroidPageWrapper(page: AndroidDriveTypePage()),
        '/route-type': (context) => AndroidPageWrapper(page: AndroidRouteTypePage()),
        '/ride': (context) => AndroidPageWrapper(page: AndroidRidePage()),
      },
    );
  }
}
