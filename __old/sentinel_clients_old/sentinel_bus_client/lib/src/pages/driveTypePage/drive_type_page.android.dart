import 'package:flutter/material.dart';

class AndroidDriveTypePage extends StatefulWidget {
  AndroidDriveTypePage({Key? key}) : super(key: key);

  @override
  _AndroidDriveTypePageState createState() => _AndroidDriveTypePageState();
}

class _AndroidDriveTypePageState extends State<AndroidDriveTypePage> {
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
                  TextButton(
                    onPressed: () => print('button-1'),
                    onLongPress: null,
                    style: TextButton.styleFrom(backgroundColor: Colors.grey[200]),
                    child: Container(
                      height: MediaQuery.of(context).size.height * 0.35,
                      width: MediaQuery.of(context).size.width * 0.80,
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.center,
                        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        children: [
                          Image.asset('assets/icons/bus_in_128.png', width: 128, height: 128, filterQuality: FilterQuality.high),
                          Padding(
                            padding: const EdgeInsets.only(bottom: 10.0),
                            child: Row(
                              crossAxisAlignment: CrossAxisAlignment.center,
                              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                              children: [
                                Image.asset('assets/icons/building_64.png', width: 64, height: 64, filterQuality: FilterQuality.high),
                                Icon(Icons.arrow_back, size: 64, color: Colors.black),
                              ],
                            ),
                          ),
                        ],
                      ),
                    ),
                  ),
                  TextButton(
                    onPressed: () => print('button-2'),
                    onLongPress: null,
                    style: TextButton.styleFrom(backgroundColor: Colors.grey[200]),
                    child: Container(
                      height: MediaQuery.of(context).size.height * 0.35,
                      width: MediaQuery.of(context).size.width * 0.80,
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.center,
                        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        children: [
                          Image.asset('assets/icons/bus_out_128.png', width: 128, height: 128, filterQuality: FilterQuality.high),
                          Padding(
                            padding: const EdgeInsets.only(bottom: 10.0),
                            child: Row(
                              crossAxisAlignment: CrossAxisAlignment.center,
                              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                              children: [
                                Image.asset('assets/icons/building_64.png', width: 64, height: 64, filterQuality: FilterQuality.high),
                                Icon(Icons.arrow_forward, size: 64, color: Colors.black),
                              ],
                            ),
                          ),
                        ],
                      ),
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
