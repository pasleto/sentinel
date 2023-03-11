// import 'package:fluent_ui/fluent_ui.dart';

import 'package:sentinel_client/providers/disposable_provider.dart';

import 'package:flutter/foundation.dart';
import 'package:universal_html/html.dart' as html;

class AppConstants extends DisposableProvider {
  final String _appName = 'Sentinel';
  String _backendServer = 'https://localhost:443'; // TODO
  // String _backendServer = 'https://sentinel.pasler.org:443'; // TODO
  String _namespace = 'client-app';
  // String _appVersion = '';

  String get appName => _appName;
  // set appName(String appName) {
  //   _appName = appName;
  //   notifyListeners();
  // }

  // String get backendServer => _backendServer;
  String get backendServer {
    if (kIsWeb) {
      if (html.window.location.hostname == 'localhost') {
        return 'https://localhost:443';
      } else {
        return html.window.location.origin;
      }
    } else {
      return _backendServer;
    }
  }

  set backendServer(String backendServer) {
    _backendServer = backendServer;
    notifyListeners();
  }

  String get namespace => _namespace;
  set namespace(String namespace) {
    _namespace = namespace;
    notifyListeners();
  }

  // String get appVersion => _appVersion;
  // set appVersion(String appVersion) {
  //   _appVersion = appVersion;
  //   notifyListeners();
  // }

  @override
  void disposeValues() {
    // ? implement disposeValues
    // _backendServer = '';
    // _namespace = '';
    // _appVersion = '';
    // notifyListeners();
  }
}
