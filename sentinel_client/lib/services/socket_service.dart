// import 'package:fluent_ui/fluent_ui.dart';
import 'package:socket_io_client/socket_io_client.dart'; // as IO;
// import 'package:sentinel_client/providers/constants_provider.dart';

class SocketService {
  // static const String _backendServer = 'https://localhost:443';
  // static const String _namespace = 'client-app';
  // static const String _connectUrl = '$_backendServer/$_namespace';
  // TODO - find better way to load connection string from constants provider

  static Socket? _socket;

  static get socket => _socket;

  static init(String connectUrl, String authToken) {
    _socket = io(
        // '${AppConstants().backendServer}/${AppConstants().namespace}',
        connectUrl,
        OptionBuilder()
            .setTransports(['websocket'])
            .setAuth({
              'token': 'Bearer $authToken', // TODO - remove Bearer ??
            })
            // .setQuery({
            //   'deviceId': deviceId,
            // })
            .enableForceNew()
            // .disableMultiplex()
            // .enableForceNewConnection()
            .disableAutoConnect()
            .build());
    print('socket initialized ...');
  }

  static connect() {
    if (!_socket!.connected) {
      _socket!.connect();
      print('socket connecting ...');
    }

    // _socket!.onConnect((_) {
    //   print('connected and listening to socket!.');
    // });

    // _socket!.onDisconnect((_) {
    //   print('disconnected from socket!.');
    // });

    // _socket!.onError((data) {
    //   print('error on socket!.');
    //   print(data);
    // });

    // _socket!.onConnectError((data) {
    //   print('connect error on socket!.');
    //   print(data);
    // });

    // When the message event 'tick' received from server, that data is added to a stream 'streamSocket'.
    // socket!.on(TimerEvents.tick.toString().split('.').last, (data) {
    //   streamSocket.addResponse(data['timer'].toString());
    // });
  }

  static clearAllListeners() {
    print('CLEAR ALL LISTENERS');
    _socket!.clearListeners();
  }

  static disconnect() async {
    _socket!.disconnect();
    // _socket!.destroy(); // ?
  }

  static dispose() async {
    _socket!.dispose();
  }
}
