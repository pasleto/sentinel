import 'package:fluent_ui/fluent_ui.dart';
import 'package:system_theme/system_theme.dart';
// import 'package:hive/hive.dart';
// import 'package:hive_flutter/hive_flutter.dart';
import 'package:flutter_web_plugins/url_strategy.dart';
import 'package:provider/provider.dart';

import 'package:sentinel_client/providers/constants_provider.dart';
import 'package:sentinel_client/providers/socket_state_provider.dart';
import 'package:sentinel_client/providers/theme_provider.dart';
import 'package:sentinel_client/providers/user_provider.dart';
import 'package:sentinel_client/providers/auth_token_provider.dart';

import 'package:sentinel_client/app.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  // setPathUrlStrategy();
  setUrlStrategy(null);
  // await Hive.initFlutter();
  // await Hive.openBox('storage');
  await SystemTheme.accentColor.load();
  runApp(
    MultiProvider(
      providers: [
        ChangeNotifierProvider(create: (_) => AppTheme()),
        ChangeNotifierProvider(create: (_) => AppConstants()),
        ChangeNotifierProvider(create: (_) => SocketState()),
        ChangeNotifierProvider(create: (_) => AuthToken()),
        ChangeNotifierProvider(create: (_) => AppUser()),
      ],
      child: const App(),
    ),
  );
}
