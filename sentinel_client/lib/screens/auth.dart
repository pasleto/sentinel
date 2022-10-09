import 'package:fluent_ui/fluent_ui.dart';
// import 'package:provider/provider.dart';

import 'login.dart';
import 'main.dart';

class AuthScreen extends StatefulWidget {
  const AuthScreen({super.key});

  @override
  State<AuthScreen> createState() => _AuthScreenState();
}

class _AuthScreenState extends State<AuthScreen> {
  Widget finalScreen = const LoginScreen(); // MainScreen() // LoginScreen()

  @override
  void initState() {
    super.initState();
  }

  @override
  void dispose() {
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return finalScreen;
  }
}
