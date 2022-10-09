import 'package:fluent_ui/fluent_ui.dart' /*hide Page*/;
// import 'package:flutter_acrylic/flutter_acrylic.dart' as flutter_acrylic;
import 'package:provider/provider.dart';

import 'providers/theme_provider.dart';
import 'screens/auth.dart';

class App extends StatelessWidget {
  const App({super.key});

  @override
  Widget build(BuildContext context) {
    // return ChangeNotifierProvider(
    //   create: (_) => AppTheme(),
    //   builder: (context, _) {
    final appTheme = context.watch<AppTheme>();
    return FluentApp(
      title: 'Sentinel',
      themeMode: appTheme.mode,
      debugShowCheckedModeBanner: false,
      color: appTheme.color,
      darkTheme: ThemeData(
        brightness: Brightness.dark,
        accentColor: appTheme.color,
        visualDensity: VisualDensity.standard,
        // focusTheme: FocusThemeData(glowFactor: is10footScreen() ? 2.0 : 0.0),
      ),
      theme: ThemeData(
        accentColor: appTheme.color,
        visualDensity: VisualDensity.standard,
        // focusTheme: FocusThemeData(glowFactor: is10footScreen() ? 2.0 : 0.0),
      ),
      locale: appTheme.locale,
      // builder: (context, child) {
      //   return Directionality(
      //     textDirection: appTheme.textDirection,
      //     child: NavigationPaneTheme(
      //       data: NavigationPaneThemeData(backgroundColor: appTheme.windowEffect != flutter_acrylic.WindowEffect.disabled ? Colors.transparent : null),
      //       child: child!,
      //     ),
      //   );
      // },
      home: const AuthScreen(),
      // initialRoute: '/',
      // routes: {'/': (context) => const AuthScreen()},
      // onUnknownRoute: (settings) {
      //   FluentPageRoute(builder: (context) => const AuthScreen());
      // },
    );
    //   },
    // );
  }
}
