import 'package:fluent_ui/fluent_ui.dart';
import 'package:flutter_acrylic/flutter_acrylic.dart' as flutter_acrylic;
import 'package:provider/provider.dart';

import 'package:sentinel_client/providers/constants_provider.dart';
import 'package:sentinel_client/providers/socket_state_provider.dart';
import 'package:sentinel_client/providers/theme_provider.dart';
import 'package:sentinel_client/providers/user_provider.dart';
import 'package:sentinel_client/providers/auth_token_provider.dart';
import 'package:sentinel_client/routes/screens/root.dart' as root_screens; // import 'screens/auth.dart';

import 'package:sentinel_client/services/socket_service.dart';
import 'package:socket_io_client/socket_io_client.dart';

class App extends StatelessWidget {
  const App({super.key});

  @override
  Widget build(BuildContext context) {
    Provider.of<AppTheme>(context, listen: false).initMode(); // load theme mode from storage

    AppConstants appConstants = context.watch<AppConstants>();
    AuthToken authToken = context.watch<AuthToken>();
    AppUser appUser = context.read<AppUser>();
    SocketState socketState = context.read<SocketState>();

    if (authToken.token.isNotEmpty && authToken.token != 'init') {
      SocketService.init('${appConstants.backendServer}/${appConstants.namespace}', authToken.token);
      Socket socket = SocketService.socket;

      socket.onConnect((_) {
        // socket.emitWithAck('login', {}, ack: (response) {
        //   if (response['status'] == 'OK') {
        //     var user = response['data']['user'];
        //     print('Setting user provider from ON CONNECT using SOCKET: ${socket.id}');
        //     appUser.setUserDataFromJson(user);
        //     print('SOCKET CONNECT: connected and listening to socket!.');
        //     socketState.socketConnected = true;
        //   } else {
        //     // TODO
        //     var msg = response['data']['message'];
        //     print(msg);
        //     print('SOCKET CONNECT: connected and listening to socket!.');
        //     socketState.socketConnected = true;
        //   }
        // });
        print('SOCKET CONNECT: connected and listening to socket!.');
        socketState.socketConnected = true;
        socket.emitWithAck('login', {}, ack: (response) {
          if (response['status'] == 'OK') {
            var user = response['data']['user'];
            print('Setting user provider from ON CONNECT using SOCKET: ${socket.id}');
            appUser.setUserDataFromJson(user);
          } else {
            // TODO
            var msg = response['data']['message'];
            print(msg);
          }
        });

        // Future.delayed(const Duration(seconds: 5), () {
        //   appUser.setRefreshing(true);
        //   socket.emitWithAck('login', {}, ack: (response) {
        //     if (response['status'] == 'OK') {
        //       var user = response['data']['user'];
        //       appUser.setUserDataFromJson(user);
        //     } else {
        //       // TODO
        //       var msg = response['data']['message'];
        //       print(msg);
        //     }
        //   });
        // });
      });
      socket.onDisconnect((_) {
        print('SOCKET DISCONNECT: disconnected from socket!.');
        socketState.socketConnected = false;
        appUser.isFetched = false;
      });
      socket.onError((data) {
        print('SOCKET ERROR: error on socket!.');
        print(data);
      });
      socket.onConnectError((data) {
        print('SOCKET CONNECT ERROR: connect error on socket!.');
        print(data);
      });

      // TODO - global sockets listeners goes here
    }

    return Consumer<AppTheme>(
      builder: (context, appTheme, child) {
        return FluentApp(
          title: appConstants.appName, // context.read<AppConstants>().appName,
          themeMode: appTheme.mode,
          debugShowCheckedModeBanner: false,
          color: appTheme.color,
          darkTheme: ThemeData(
            brightness: Brightness.dark,
            accentColor: appTheme.color,
            visualDensity: VisualDensity.standard,
            focusTheme: const FocusThemeData(glowFactor: 0.0),
          ),
          theme: ThemeData(
            accentColor: appTheme.color,
            visualDensity: VisualDensity.standard,
            focusTheme: const FocusThemeData(glowFactor: 0.0),
          ),
          locale: appTheme.locale,
          builder: (context, child) {
            return Directionality(
              textDirection: appTheme.textDirection,
              child: NavigationPaneTheme(
                data: NavigationPaneThemeData(backgroundColor: appTheme.windowEffect != flutter_acrylic.WindowEffect.disabled ? Colors.transparent : null),
                child: child!,
              ),
            );
          },
          home: const root_screens.AuthScreen(),
          // initialRoute: '/',
          // routes: {'/': (context) => const root_screens.AuthScreen()},
        );
      },
    );
  }
}
