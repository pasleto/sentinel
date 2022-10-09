import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/screens/main.dart';

class LoginPage extends StatefulWidget {
  const LoginPage({super.key});

  @override
  State<LoginPage> createState() => _LoginPageState();
}

class _LoginPageState extends State<LoginPage> {
  bool _isLoginRequestLoading = false;
  bool _obscurePassword = true;
  final TextEditingController _controllerUsername = TextEditingController();
  final TextEditingController _controllerPassword = TextEditingController();

  @override
  Widget build(BuildContext context) {
    return Center(
      child: SizedBox(
        width: 300,
        height: 320,
        child: Column(
          mainAxisAlignment: MainAxisAlignment.start,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            Padding(
              padding: const EdgeInsets.only(bottom: 24.0),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.center,
                crossAxisAlignment: CrossAxisAlignment.center,
                children: [
                  Padding(
                    padding: const EdgeInsets.only(right: 16.0, bottom: 4.0),
                    child: Image.asset('assets/logo/Logo_64.png', height: 48, width: 48, filterQuality: FilterQuality.high, cacheHeight: 48, cacheWidth: 48),
                  ),
                  const Text('Sentinel', style: TextStyle(fontSize: 36)),
                ],
              ),
            ),
            !_isLoginRequestLoading
                ? Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    crossAxisAlignment: CrossAxisAlignment.center,
                    children: [
                      Padding(
                        padding: const EdgeInsets.symmetric(horizontal: 32.0, vertical: 8.0),
                        child: TextFormBox(
                          enabled: !_isLoginRequestLoading,
                          prefix: const Padding(
                            padding: EdgeInsets.symmetric(horizontal: 8.0),
                            child: Icon(FluentIcons.signin),
                          ),
                          minHeight: 40,
                          textAlignVertical: TextAlignVertical.center,
                          autofocus: true,
                          autocorrect: false,
                          keyboardType: TextInputType.name,
                          controller: _controllerUsername,
                        ),
                      ),
                      Padding(
                        padding: const EdgeInsets.symmetric(horizontal: 32.0, vertical: 8.0),
                        child: TextFormBox(
                          enabled: !_isLoginRequestLoading,
                          prefix: const Padding(
                            padding: EdgeInsets.symmetric(horizontal: 8.0),
                            child: Icon(FluentIcons.password_field),
                          ),
                          minHeight: 40,
                          textAlignVertical: TextAlignVertical.center,
                          obscureText: _obscurePassword,
                          autocorrect: false,
                          keyboardType: TextInputType.visiblePassword,
                          controller: _controllerPassword,
                          onFieldSubmitted: (v) => {formSubmit()},
                          // suffix: MouseRegion(
                          //   cursor: SystemMouseCursors.click,
                          //   child: GestureDetector(
                          suffix: GestureDetector(
                            onTap: () {
                              setState(() {
                                _obscurePassword = !_obscurePassword;
                              });
                            },
                            child: Padding(
                              padding: const EdgeInsets.symmetric(horizontal: 8.0),
                              child: Icon(_obscurePassword ? FluentIcons.hide3 : FluentIcons.red_eye),
                            ),
                          ),
                        ),
                        // ),
                      ),
                      Padding(
                        padding: const EdgeInsets.symmetric(horizontal: 32.0, vertical: 24.0),
                        // child: MouseRegion(
                        //   cursor: SystemMouseCursors.click,
                        child: Button(
                          child: Row(
                            mainAxisAlignment: MainAxisAlignment.center,
                            crossAxisAlignment: CrossAxisAlignment.center,
                            children: const [
                              Padding(
                                padding: EdgeInsets.symmetric(vertical: 5.0, horizontal: 8.0),
                                child: Text('Login'),
                              ),
                            ],
                          ),
                          onPressed: () => {formSubmit()},
                        ),
                      ),
                      // ),
                    ],
                  )
                : const Padding(
                    padding: EdgeInsets.symmetric(vertical: 72.0, horizontal: 36.0),
                    child: ProgressBar(),
                  ),
          ],
        ),
      ),
    );
  }

  void formSubmit() async {
    setState(() {
      _isLoginRequestLoading = true;
    });

    final navigator = Navigator.of(context);

    debugPrint('Form Submited');
    debugPrint(_controllerUsername.text);
    debugPrint(_controllerPassword.text);

    await Future.delayed(const Duration(seconds: 2));

    navigator.pushAndRemoveUntil(FluentPageRoute(builder: (context) => const MainScreen()), (Route<dynamic> route) => false); // AuthScreen
  }
}
