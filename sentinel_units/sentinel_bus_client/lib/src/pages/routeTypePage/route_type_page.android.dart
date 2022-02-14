import 'package:flutter/material.dart';

class AndroidRouteTypePage extends StatefulWidget {
  AndroidRouteTypePage({Key? key}) : super(key: key);

  @override
  _AndroidRouteTypePageState createState() => _AndroidRouteTypePageState();
}

class _AndroidRouteTypePageState extends State<AndroidRouteTypePage> {
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
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.center,
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: [
                  // TODO Generated scrollable list from fetched Routes
                  TextButton(
                    onPressed: () => print('button-1'),
                    child: Container(
                      height: 100,
                      width: MediaQuery.of(context).size.width * 0.80,
                      child: Image.asset('assets/logo/Logo_128.png', width: 96, height: 96, filterQuality: FilterQuality.high),
                    ),
                  ),
                  TextButton(
                    onPressed: () => print('button-2'),
                    child: Container(
                      height: 100,
                      width: MediaQuery.of(context).size.width * 0.80,
                      child: Image.asset('assets/logo/Logo_128.png', width: 96, height: 96, filterQuality: FilterQuality.high),
                    ),
                  ),
                  TextButton(
                    onPressed: () => print('button-3'),
                    child: Container(
                      height: 100,
                      width: MediaQuery.of(context).size.width * 0.80,
                      child: Image.asset('assets/logo/Logo_128.png', width: 96, height: 96, filterQuality: FilterQuality.high),
                    ),
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }
}
