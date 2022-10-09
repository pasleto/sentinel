import 'package:fluent_ui/fluent_ui.dart';
import 'package:url_strategy/url_strategy.dart';
import 'package:provider/provider.dart';

import 'providers/theme_provider.dart';
import 'providers/user_provider.dart';

import 'app.dart';

const double kOpenNavigationPaneWidth = 250;

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  setPathUrlStrategy();
  runApp(
    MultiProvider(
      providers: [
        ChangeNotifierProvider(create: (_) => AppTheme()),
        ChangeNotifierProvider(create: (_) => AppUser()),
      ],
      child: const App(),
    ),
  );
}
