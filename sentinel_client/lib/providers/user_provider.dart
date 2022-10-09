import 'package:fluent_ui/fluent_ui.dart';

class AppUser with ChangeNotifier {
  bool _isLogged = false;
  bool get isLogged => _isLogged;
  set isLogged(bool isLogged) {
    _isLogged = isLogged;
    notifyListeners();
  }

  String _username = '';
  String get username => _username;
  set username(String username) {
    _username = username;
    notifyListeners();
  }

  String _name = '';
  String get name => _name;
  set name(String name) {
    _name = name;
    notifyListeners();
  }

  String _surname = '';
  String get surname => _surname;
  set surname(String surname) {
    _surname = surname;
    notifyListeners();
  }

  String _imageString = '';
  String get imageString => _imageString;
  // Image get image => _imageString; // TODO - convert imageString to something usable for image widget
  set imageString(String imageString) {
    _imageString = imageString;
    notifyListeners();
  }
}
