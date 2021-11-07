import 'package:flutter/material.dart';
import 'package:universal_platform/universal_platform.dart';
import 'package:bitsdojo_window/bitsdojo_window.dart';

import './src/apps/windows.dart';
import './src/apps/mac.dart';
import './src/apps/linux.dart';
import './src/apps/web.dart';
import './src/apps/android.dart';
import './src/apps/ios.dart';
import './src/apps/unsupported.dart';

void main() {
  if (UniversalPlatform.isWindows) {
    runApp(WindowsApp());
  } else if (UniversalPlatform.isMacOS) {
    runApp(MacApp());
  } else if (UniversalPlatform.isLinux) {
    runApp(LinuxApp());
  } else if (UniversalPlatform.isWeb) {
    runApp(WebApp());
  } else if (UniversalPlatform.isAndroid) {
    runApp(AndroidApp());
  } else if (UniversalPlatform.isIOS) {
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
