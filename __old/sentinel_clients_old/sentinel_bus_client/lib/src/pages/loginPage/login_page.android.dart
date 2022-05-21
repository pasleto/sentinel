import 'package:flutter/material.dart';

class AndroidLoginPage extends StatefulWidget {
  AndroidLoginPage({Key? key}) : super(key: key);

  @override
  _AndroidLoginPageState createState() => _AndroidLoginPageState();
}

class _AndroidLoginPageState extends State<AndroidLoginPage> {
  // TextEditingController _controllerPassword = TextEditingController();

  // bool _isLoginRequestLoading = false;

  // void _handleLoginRequest() async {
  // setState(() {
  //   this._isLoginRequestLoading = true;
  // });

  // print('login request');
  // print('password: ${_controllerPassword.text}');
  // }

  // void initState() {
  //   super.initState();
  // }

  @override
  void dispose() {
    // _controllerPassword.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(
        mainAxisSize: MainAxisSize.max,
        crossAxisAlignment: CrossAxisAlignment.center,
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Expanded(
            child: Container(
              alignment: Alignment.center,
              // padding: const EdgeInsets.only(bottom: 40.0),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.center,
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Padding(
                    padding: const EdgeInsets.only(bottom: 32.0),
                    child: Image.asset('assets/logo/Logo_128.png', width: 96, height: 96, filterQuality: FilterQuality.high),
                  ),
                  Container(
                    height: 65,
                    width: 250,
                    child: Padding(
                      padding: const EdgeInsets.all(8.0),
                      child: Text(
                        'Scan Bus Driver Card',
                        textAlign: TextAlign.center,
                        style: TextStyle(
                          fontSize: 20,
                          fontWeight: FontWeight.w600,
                          color: Colors.red,
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
    );
  }
}
