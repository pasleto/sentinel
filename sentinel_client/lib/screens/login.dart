import 'package:fluent_ui/fluent_ui.dart';
import '../widgets/appbar.dart';
import '../pages/login.dart';

class LoginScreen extends StatefulWidget {
  const LoginScreen({super.key});

  @override
  State<LoginScreen> createState() => _LoginScreenState();
}

class _LoginScreenState extends State<LoginScreen> {
  final viewKey = GlobalKey();

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
    return SafeArea(
      child: NavigationView(
        key: viewKey,
        appBar: appBarLogin(),
        content: const LoginPage(),
      ),
    );
  }
}
