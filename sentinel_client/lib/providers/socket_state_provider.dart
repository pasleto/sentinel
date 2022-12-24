import 'package:sentinel_client/providers/disposable_provider.dart';

class SocketState extends DisposableProvider {
  bool _socketConnected = false;
  bool get socketConnected => _socketConnected;
  set socketConnected(bool socketConnected) {
    _socketConnected = socketConnected;
    notifyListeners();
  }

  @override
  void disposeValues() {
    _socketConnected = false;
    notifyListeners();
  }
}
