import 'package:fluent_ui/fluent_ui.dart';
import 'package:provider/provider.dart';

import 'package:sentinel_client/providers/disposable_provider.dart';

// import 'theme_provider.dart';
import 'package:sentinel_client/providers/constants_provider.dart';
import 'package:sentinel_client/providers/socket_state_provider.dart';
import 'package:sentinel_client/providers/user_provider.dart';
import 'package:sentinel_client/providers/auth_token_provider.dart';

class AppProviders {
  static List<DisposableProvider> getDisposableProviders(BuildContext context) {
    return [
      // Provider.of<AppTheme>(context, listen: false),
      Provider.of<AppConstants>(context, listen: false),
      Provider.of<SocketState>(context, listen: false),
      Provider.of<AuthToken>(context, listen: false),
      Provider.of<AppUser>(context, listen: false),
      //...other disposable providers
    ];
  }

  static void disposeAllDisposableProviders(BuildContext context) {
    getDisposableProviders(context).forEach((disposableProvider) {
      disposableProvider.disposeValues();
    });
  }
}
