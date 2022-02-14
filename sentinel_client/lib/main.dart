import 'dart:io';
import 'dart:async';

import 'package:flutter/material.dart';
import 'package:universal_platform/universal_platform.dart';
import 'package:bitsdojo_window/bitsdojo_window.dart';
import 'package:url_strategy/url_strategy.dart';

import 'package:hive/hive.dart';
import 'package:hive_flutter/hive_flutter.dart';
import 'package:path_provider/path_provider.dart' as pathProvider;

import 'package:sentinel_client/src/apps/windows.dart';
import 'package:sentinel_client/src/apps/mac.dart';
import 'package:sentinel_client/src/apps/linux.dart';
import 'package:sentinel_client/src/apps/web.dart';
import 'package:sentinel_client/src/apps/android.dart';
import 'package:sentinel_client/src/apps/ios.dart';
import 'package:sentinel_client/src/apps/unsupported.dart';

// void main() {
Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  setPathUrlStrategy();

  if (UniversalPlatform.isWeb) {
    runApp(WebApp());
  } else if (UniversalPlatform.isWindows) {
    Directory directory = await pathProvider.getApplicationSupportDirectory();

    runApp(WindowsApp());
  } else if (UniversalPlatform.isMacOS) {
    Directory directory = await pathProvider.getApplicationSupportDirectory();

    runApp(MacApp());
  } else if (UniversalPlatform.isLinux) {
    Directory directory = await pathProvider.getApplicationSupportDirectory();

    runApp(LinuxApp());
  } else if (UniversalPlatform.isAndroid) {
    Directory directory = await pathProvider.getApplicationSupportDirectory();

    runApp(AndroidApp());
  } else if (UniversalPlatform.isIOS) {
    Directory directory = await pathProvider.getApplicationSupportDirectory();

    runApp(IosApp());
  } else {
    runApp(UnsupportedApp());
  }

  if (UniversalPlatform.isWindows || UniversalPlatform.isMacOS || UniversalPlatform.isLinux) {
    doWhenWindowReady(() {
      appWindow.size = Size(1000, 600);
      appWindow.minSize = Size(800, 600);
      appWindow.alignment = Alignment.center;
      appWindow.title = 'Sentinel';
      appWindow.show();
    });
  }
}
