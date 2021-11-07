import 'dart:io';
import 'package:flutter/material.dart';

class UnsupportedPage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: <Widget>[
            Row(
              children: [
                Padding(
                  padding: const EdgeInsets.only(left: 48.0),
                  child: Image.asset('assets/icons/cms_icon_256.png', width: 72, height: 72, filterQuality: FilterQuality.high),
                ),
                Image.asset('assets/images/cms_title.png', scale: 2.0, filterQuality: FilterQuality.high),
              ],
              crossAxisAlignment: CrossAxisAlignment.center,
              mainAxisAlignment: MainAxisAlignment.center,
              mainAxisSize: MainAxisSize.max,
            ),
            Padding(
              padding: const EdgeInsets.only(bottom: 48.0, top: 16.0, left: 16.0, right: 16.0),
              child: Text(
                'This platform is not supported!',
                textAlign: TextAlign.center,
                style: TextStyle(
                  fontSize: 20,
                  fontWeight: FontWeight.bold,
                  color: Colors.black,
                ),
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(16.0),
              child: TextButton(
                onPressed: () => exit(0),
                child: Padding(
                  padding: const EdgeInsets.only(bottom: 6.0, top: 2.0, left: 24.0, right: 24.0),
                  child: Text(
                    'EXIT',
                    textAlign: TextAlign.center,
                    style: TextStyle(
                      fontSize: 20,
                      fontWeight: FontWeight.bold,
                      color: Colors.red,
                    ),
                  ),
                ),
                style: TextButton.styleFrom(
                  padding: const EdgeInsets.all(12.0),
                  primary: Colors.grey,
                  backgroundColor: Colors.grey[300],
                ),
              ),
            )
          ],
        ),
      ),
    );
  }
}
