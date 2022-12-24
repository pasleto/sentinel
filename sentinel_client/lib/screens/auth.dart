import 'package:fluent_ui/fluent_ui.dart';
import 'package:provider/provider.dart';
import 'package:jwt_decode/jwt_decode.dart';
import 'package:sentinel_client/providers/app_providers.dart';
import 'package:sentinel_client/providers/socket_state_provider.dart';

import 'package:sentinel_client/providers/user_provider.dart';
import 'package:sentinel_client/providers/auth_token_provider.dart';
import 'package:sentinel_client/routes/screens/root.dart' as root_screens;
import 'package:sentinel_client/services/socket_service.dart';
import 'package:sentinel_client/widgets/toasts.dart';

class AuthScreen extends StatelessWidget {
  const AuthScreen({super.key});

  @override
  Widget build(BuildContext context) {
    AppUser appUser = context.watch<AppUser>();
    AuthToken authToken = context.watch<AuthToken>();
    SocketState socketState = context.watch<SocketState>();

    if (authToken.token.isNotEmpty && authToken.token != 'init') {
      if (!Jwt.isExpired(authToken.token)) {
        // TODO - handle FormatException: Invalid token
        if (appUser.isLogged) {
          if (socketState.socketConnected) {
            if (appUser.isFetched) {
              AppToasts.dismissNamed('LoaderPanel-SocketDisconnect');
              AppToasts.dismissNamed('LoaderPanel-FetchingUserData');
              return const root_screens.MainScreen();
            } else {
              AppToasts.fullscreenLoaderPanel(
                context: context,
                messageText: 'Refreshing user data ...',
                panelName: 'LoaderPanel-FetchingUserData',
                appBar: true, // ?
                logoutButton: true, // ?
              );
              return const root_screens.MainScreen();
            }
          } else {
            AppToasts.fullscreenLoaderPanel(
              context: context,
              messageText: 'Lost connection! Trying to reconnect ...',
              panelName: 'LoaderPanel-SocketDisconnect',
              appBar: true,
              logoutButton: true,
            );
            SocketService.connect();
            return const root_screens.MainScreen();
            // return const root_screens.LoaderScreen(showLogoutButton: true, showAppbar: true, message: 'Establishing socket connection ...');
          }
        } else {
          if (socketState.socketConnected) {
            return const root_screens.LoaderScreen(
              message: 'Fetching user data ...',
              showAppbar: true, // ?
              showLogoutButton: true, // ?
            );
          } else {
            SocketService.connect();
            return const root_screens.LoaderScreen(
              showLogoutButton: true,
              showAppbar: true,
              message: 'Establishing socket connection ...',
            );
          }
        }
        // if (socketState.socketConnected) {
        //   if (appUser.isLogged) {
        //     return const root_screens.MainScreen();
        //   } else {
        //     return const root_screens.LoaderScreen(message: 'Fetching user data ...');
        //   }
        // } else {
        //   if (appUser.isLogged) {
        //     SocketService.connect();
        //     return const root_screens.LoaderScreen(showLogoutButton: true, showAppbar: true, message: 'Establishing socket connection ...');
        //   } else {
        //     SocketService.connect();
        //     return const root_screens.LoaderScreen(message: 'Establishing socket connection ...');
        //   }
        // }
      } else {
        // logout
        Provider.of<AuthToken>(context, listen: false).deleteFromStorage();
        SocketService.disconnect();
        SocketService.dispose();
        AppProviders.disposeAllDisposableProviders(context);
        // notify
        AppToasts.dismissAll(atSameTime: true);
        AppToasts.warningNotification(
          context: context,
          titleText: 'Authentication Token Expired',
          messageText: 'You have been logged out. Please log in again to continue working.',
          closeButton: false,
        );
        // return login page
        return const root_screens.LoginScreen();
      }
    } else {
      if (authToken.token == 'init') {
        authToken.init();
        return const root_screens.LoaderScreen(message: 'Checking for existing login state ...');
      } else {
        return const root_screens.LoginScreen();
      }
    }
  }
}
