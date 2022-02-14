import 'dart:io';
import 'dart:async';

import 'package:flutter/material.dart';
import 'package:universal_platform/universal_platform.dart';
import 'package:url_strategy/url_strategy.dart';

import 'package:hive/hive.dart';
import 'package:hive_flutter/hive_flutter.dart';
import 'package:path_provider/path_provider.dart' as pathProvider;

import 'package:sentinel_bus_client/src/apps/android.dart';
import 'package:sentinel_bus_client/src/apps/unsupported.dart';

// void main() {
Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  setPathUrlStrategy();

  if (UniversalPlatform.isAndroid) {
    Directory directory = await pathProvider.getApplicationSupportDirectory();
    print(directory);

    runApp(AndroidApp());
  } else {
    runApp(UnsupportedApp());
  }
}
