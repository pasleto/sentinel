import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/widgets/page.dart';
import 'package:provider/provider.dart';
import 'package:sentinel_client/providers/constants_provider.dart';
import 'package:sentinel_client/providers/auth_token_provider.dart';
import 'package:sentinel_client/widgets/toasts.dart';

class LoginPage extends StatefulWidget {
  const LoginPage({super.key});

  @override
  State<LoginPage> createState() => _LoginPageState();
}

class _LoginPageState extends State<LoginPage> with PageMixin {
  bool _isLoginRequestLoading = false;
  bool _obscurePassword = true;
  final TextEditingController _controllerUsername = TextEditingController();
  final TextEditingController _controllerPassword = TextEditingController();
  final FocusNode _focusNodePassword = FocusNode();

  @override
  void initState() {
    super.initState();
  }

  @override
  void dispose() {
    _controllerUsername.dispose();
    _controllerPassword.dispose();
    _focusNodePassword.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    AppConstants appConstants = Provider.of<AppConstants>(context);
    AuthToken authToken = Provider.of<AuthToken>(context);

    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          Expanded(
            child: SizedBox(
              width: 300,
              height: 320,
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
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
                          child:
                              Image.asset('assets/logo/Logo_64.png', height: 48, width: 48, filterQuality: FilterQuality.high, cacheHeight: 48, cacheWidth: 48),
                        ),
                        Consumer<AppConstants>(
                          builder: (context, constant, child) {
                            return Text(constant.appName, style: const TextStyle(fontSize: 36));
                          },
                        ),
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
                                onFieldSubmitted: (v) => {_focusNodePassword.requestFocus()},
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
                                focusNode: _focusNodePassword,
                                onFieldSubmitted: (v) => {_formSubmit(appConstants, authToken)},
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
                            ),
                            Padding(
                              padding: const EdgeInsets.symmetric(horizontal: 32.0, vertical: 24.0),
                              child: Button(
                                onPressed: () => {_formSubmit(appConstants, authToken)},
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
                              ),
                            ),
                          ],
                        )
                      : const Padding(
                          // padding: EdgeInsets.symmetric(vertical: 72.0, horizontal: 36.0),
                          padding: EdgeInsets.symmetric(vertical: 96.0, horizontal: 36.0),
                          child: ProgressBar(),
                        ),
                ],
              ),
            ),
          ),
          const Padding(
            padding: EdgeInsets.only(bottom: 24.0, left: 24.0, right: 24.0),
            child: Text(
              'Powered by Sentinel',
              maxLines: 1,
              textAlign: TextAlign.center,
              style: TextStyle(
                fontStyle: FontStyle.italic,
                fontSize: 12,
                fontWeight: FontWeight.w100,
                color: Color.fromARGB(192, 128, 128, 128),
                height: 1.0,
              ),
            ),
          ),
        ],
      ),
    );
  }

  void _formSubmit(AppConstants appConstants, AuthToken authToken) async {
    setState(() {
      _isLoginRequestLoading = true;
    });

    if (_controllerUsername.text.isEmpty || _controllerPassword.text.isEmpty) {
      AppToasts.dismissAll(atSameTime: true);
      AppToasts.warningNotification(context: context, duration: const Duration(seconds: 3), messageText: 'Username and password required!');
      setState(() {
        _isLoginRequestLoading = false;
      });
    } else {
      authToken.fetch(appConstants.backendServer, appConstants.namespace, _controllerUsername.text, _controllerPassword.text, (status, result) {
        if (status == 'OK') {
          AppToasts.dismissAll(atSameTime: true);
          AppToasts.successNotification(context: context, duration: const Duration(seconds: 3), messageText: 'Login successful!');
          result();
        } else if (status == 'NOK') {
          AppToasts.dismissAll(atSameTime: true);
          AppToasts.warningNotification(context: context, duration: const Duration(seconds: 3), messageText: result);
          setState(() {
            _isLoginRequestLoading = false;
          });
        } else {
          AppToasts.dismissAll(atSameTime: true);
          AppToasts.errorNotification(context: context, duration: const Duration(seconds: 3), messageText: 'Unable to contact backend server!');
          setState(() {
            _isLoginRequestLoading = false;
          });
        }
      });
    }
  }
}
