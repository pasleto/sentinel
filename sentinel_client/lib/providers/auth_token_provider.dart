import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:sentinel_client/providers/disposable_provider.dart';
import 'package:sentinel_client/services/storage_service.dart';

class AuthToken extends DisposableProvider {
  String _token = 'init';
  String get token => _token;
  set token(String token) {
    _token = token;
    notifyListeners();
  }

  void init() async {
    String? savedToken = await StorageService.readSecureData('authToken');
    if (savedToken != null) {
      _token = savedToken;
    } else {
      _token = '';
    }
    notifyListeners();
  }

  void _setToStorage(String token) async {
    await StorageService.writeSecureData('authToken', token);
  }

  void deleteFromStorage() async {
    await StorageService.deleteSecureData('authToken');
  }

  void fetch(String server, String namespace, String username, String password, Function callback) async {
    try {
      var response = await http.post(
        Uri.parse('$server/api/$namespace/login-password'),
        // Uri.parse('https://localhost:443/api/client-app/login-password'), // ? - this should be only /api/client-app/login-password on production
        headers: <String, String>{
          'Content-Type': 'application/json; charset=UTF-8',
        },
        body: jsonEncode(<String, String>{
          'username': username,
          'password': password,
        }),
      );
      if (response.statusCode == 200) {
        var body = jsonDecode(response.body);
        _setToStorage(body['data']['token']);
        _token = body['data']['token'];
        // callback(true, notifyListeners);
        callback('OK', notifyListeners);
      } else {
        var body = jsonDecode(response.body);
        // callback(false, body['data']['message']);
        callback('NOK', body['data']['message']);
      }
    } catch (e) {
      // callback(false, e);
      callback('ERR', e);
    }
  }

  @override
  void disposeValues() {
    // ? implement disposeValues
    _token = '';
    notifyListeners();
  }
}
