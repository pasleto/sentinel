import 'dart:io';
import 'package:flutter/material.dart';

class UnsupportedApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Sentinel Bus',
      home: Scaffold(
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: <Widget>[
              Image.asset('assets/logo/Logo_Horizontal_128.png', scale: 1.7, filterQuality: FilterQuality.high),
              Padding(
                padding: const EdgeInsets.only(bottom: 64.0, top: 96.0, left: 16.0, right: 16.0),
                child: Text(
                  'This platform is not supported!',
                  textAlign: TextAlign.center,
                  style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: Colors.black),
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
                      style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: Colors.red),
                    ),
                  ),
                  style: TextButton.styleFrom(
                    padding: const EdgeInsets.all(12.0),
                    primary: Colors.grey,
                    backgroundColor: Colors.grey[300],
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
