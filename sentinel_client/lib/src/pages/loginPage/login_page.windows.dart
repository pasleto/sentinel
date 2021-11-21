import 'package:flutter/material.dart';
import 'package:bitsdojo_window/bitsdojo_window.dart';

class WindowsLoginPage extends StatefulWidget {
  WindowsLoginPage({Key? key}) : super(key: key);

  @override
  _WindowsLoginPageState createState() => _WindowsLoginPageState();
}

final buttonColors = WindowButtonColors(
  iconNormal: Colors.black,
  mouseOver: Colors.black12,
  mouseDown: Colors.black26,
  iconMouseOver: Colors.white,
  iconMouseDown: Colors.white,
);

final closeButtonColors = WindowButtonColors(
  iconNormal: Colors.black,
  mouseOver: Colors.redAccent[700],
  mouseDown: Colors.red[900],
  iconMouseOver: Colors.white,
  iconMouseDown: Colors.white,
);

class _WindowsLoginAppBar extends StatelessWidget implements PreferredSizeWidget {
  @override
  Widget build(BuildContext context) {
    return AppBar(
      automaticallyImplyLeading: false,
      titleSpacing: 10.0,
      title: Align(
        alignment: Alignment.centerLeft,
        child: WindowTitleBarBox(
          child: Row(
            children: [
              MoveWindow(
                child: Row(
                  children: [
                    Padding(
                      padding: const EdgeInsets.only(top: 7.0, bottom: 7.0, right: 10.0),
                      child: Image.asset('assets/logo/Logo_Black_32.png', filterQuality: FilterQuality.high),
                    ),
                    Padding(
                      padding: const EdgeInsets.only(top: 7.0, bottom: 7.0),
                      child: Image.asset('assets/logo/Logo_Text_Black_16.png', filterQuality: FilterQuality.high, height: 12),
                    ),
                  ],
                ),
              ),
              Expanded(child: MoveWindow()),
            ],
          ),
        ),
      ),
      actions: [
        Row(children: [
          MinimizeWindowButton(colors: buttonColors),
          MaximizeWindowButton(colors: buttonColors),
          CloseWindowButton(colors: closeButtonColors),
        ]),
      ],
    );
  }

  @override
  Size get preferredSize {
    return new Size.fromHeight(30);
  }
}

class _WindowsLoginPageState extends State<WindowsLoginPage> {
  TextEditingController _controllerUsername = TextEditingController();
  TextEditingController _controllerPassword = TextEditingController();

  bool _isLoginRequestLoading = false;

  void _handleLoginRequest() async {
    setState(() {
      this._isLoginRequestLoading = true;
    });

    print('login request');
    print('username: ${_controllerUsername.text}');
    print('password: ${_controllerPassword.text}');
  }

  // void initState() {
  //   super.initState();
  // }

  @override
  void dispose() {
    _controllerUsername.dispose();
    _controllerPassword.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: _WindowsLoginAppBar(),
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.max,
          crossAxisAlignment: CrossAxisAlignment.center,
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Expanded(
              child: Container(
                alignment: Alignment.center,
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.center,
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    Padding(
                      padding: const EdgeInsets.only(bottom: 32.0),
                      child: Image.asset('assets/logo/Logo_128.png', width: 96, height: 96, filterQuality: FilterQuality.high),
                    ),
                    Container(
                      height: 55,
                      width: 250,
                      child: Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: TextField(
                          autofocus: true,
                          enabled: (this._isLoginRequestLoading) ? false : true,
                          keyboardType: TextInputType.text,
                          controller: this._controllerUsername,
                          style: TextStyle(),
                          textAlignVertical: TextAlignVertical.center,
                          decoration: InputDecoration(
                            filled: true,
                            // fillColor: Color(0x40FAFAFA),
                            prefixIcon: Padding(
                              padding: const EdgeInsets.only(top: 5, bottom: 5, left: 10, right: 15),
                              child: Image.asset('assets/icons/user_25.png', width: 25, height: 25, filterQuality: FilterQuality.high),
                            ),
                            // suffixText: loginPageWords.contiwanDomain,
                            // icon: Padding(
                            //   padding: const EdgeInsets.only(left: 8.0),
                            //   child: Image.asset('assets/icons/user_icon_filled_60.png', width: 25, height: 25, filterQuality: FilterQuality.high),
                            // ),
                            // hintText: 'Username',
                            // hintStyle: TextStyle(color: Colors.black26),
                          ),
                        ),
                      ),
                    ),
                    Container(
                      height: 55,
                      width: 250,
                      child: Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: TextField(
                          autofocus: false,
                          enabled: (this._isLoginRequestLoading) ? false : true,
                          keyboardType: TextInputType.visiblePassword,
                          obscureText: true,
                          controller: this._controllerPassword,
                          onSubmitted: (value) => this._handleLoginRequest(),
                          style: TextStyle(),
                          textAlignVertical: TextAlignVertical.center,
                          decoration: InputDecoration(
                            filled: true,
                            prefixIcon: Padding(
                              padding: const EdgeInsets.only(top: 5, bottom: 5, left: 10, right: 15),
                              child: Image.asset('assets/icons/lock_25.png', width: 25, height: 25, filterQuality: FilterQuality.high),
                            ),
                            // hintText: 'Password',
                            // hintStyle: TextStyle(color: Colors.black26),
                          ),
                        ),
                      ),
                    ),
                    Container(
                      height: 55,
                      width: 250,
                      decoration: BoxDecoration(
                        borderRadius: BorderRadius.circular(20),
                      ),
                      child: Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: TextButton(
                          onPressed: (this._isLoginRequestLoading) ? null : this._handleLoginRequest,
                          child: (this._isLoginRequestLoading)
                              ? Container(child: LinearProgressIndicator(minHeight: 3), height: 3, width: 156)
                              : Padding(
                                  padding: const EdgeInsets.only(bottom: 4.0),
                                  child: Text('Login', textAlign: TextAlign.center, style: TextStyle(color: Colors.black, fontSize: 20)),
                                ),
                          style: TextButton.styleFrom(
                            primary: Colors.black26,
                            backgroundColor: (this._isLoginRequestLoading) ? Colors.transparent : Colors.black12,
                          ),
                        ),
                      ),
                    ),
                  ],
                ),
              ),
            ),
            Container(
              height: 45,
              alignment: Alignment.center,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.center,
                mainAxisAlignment: MainAxisAlignment.start,
                children: [
                  Text('Sentinel', style: TextStyle(fontWeight: FontWeight.w600, fontSize: 13)),
                  Text('Copyright © 2021 Tomáš Pásler', style: TextStyle(fontWeight: FontWeight.w500, fontStyle: FontStyle.italic, fontSize: 12)),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
