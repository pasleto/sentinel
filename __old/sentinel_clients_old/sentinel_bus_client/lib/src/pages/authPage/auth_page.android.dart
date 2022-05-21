import 'package:flutter/material.dart';

// import 'package:flutter_svg/flutter_svg.dart';

class AndroidAuthPage extends StatefulWidget {
  AndroidAuthPage({Key? key}) : super(key: key);

  @override
  _AndroidAuthPageState createState() => _AndroidAuthPageState();
}

class _AndroidAuthPageState extends State<AndroidAuthPage> {
  void initState() {
    super.initState();
  }

  void _handleLogin() {
    // read card uid from serial usb reader
    // compare card uid to list of driver cards
    // if card is driver card -> navigate to home
    // if card is not driver card -> throw error notification
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
                    padding: const EdgeInsets.all(16.0),
                    child: Image.asset('assets/logo/Logo_128.png', width: 96, height: 96, filterQuality: FilterQuality.high),
                  ),
                  Padding(
                    padding: const EdgeInsets.all(48.0),
                    child: Container(child: LinearProgressIndicator(minHeight: 4), height: 4, width: 196),
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
