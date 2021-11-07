import 'package:flutter/material.dart';

class MacHomePage extends StatefulWidget {
  MacHomePage({Key? key}) : super(key: key);

  @override
  _MacHomePageState createState() => _MacHomePageState();
}

class _MacHomePageState extends State<MacHomePage> {
  // int _counter = 0;

  // void _incrementCounter() {
  //   setState(() {
  //     _counter++;
  //   });
  // }

  @override
  Widget build(BuildContext context) {
    // return Scaffold(
    //   appBar: AppBar(
    //     title: Text(widget.title),
    //   ),
    //   body: Center(
    //     child: Column(
    //       mainAxisAlignment: MainAxisAlignment.center,
    //       children: <Widget>[
    //         Text(
    //           'You have pushed the button this many times:',
    //         ),
    //         Text(
    //           '$_counter',
    //           style: Theme.of(context).textTheme.headline4,
    //         ),
    //       ],
    //     ),
    //   ),
    //   floatingActionButton: FloatingActionButton(
    //     onPressed: _incrementCounter,
    //     tooltip: 'Increment',
    //     child: Icon(Icons.add),
    //   ),
    // );
    return Column(
      children: [
        Text('Hello'),
        Text('Mac'),
        Text('Home'),
        Text('Page'),
      ],
    );
  }
}
