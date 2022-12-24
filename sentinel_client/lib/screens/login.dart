import 'package:fluent_ui/fluent_ui.dart';

import 'package:sentinel_client/widgets/appbar.dart';
import 'package:sentinel_client/routes/pages/root.dart' as root_pages;

class LoginScreen extends StatelessWidget {
  const LoginScreen({super.key});

  // @override
  @override
  Widget build(BuildContext context) {
    return SafeArea(
      child: NavigationView(
        key: GlobalKey(),
        appBar: appBarLogin(),
        transitionBuilder: (child, animation) {
          return DrillInPageTransition(animation: animation, child: child);
        },
        content: const root_pages.LoginPage(),
      ),
    );
  }
}
