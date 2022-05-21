import 'package:flutter/material.dart';

class AndroidRidePage extends StatefulWidget {
  AndroidRidePage({Key? key}) : super(key: key);

  @override
  _AndroidRidePageState createState() => _AndroidRidePageState();
}

class _AndroidRidePageState extends State<AndroidRidePage> {
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
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Container(
                    height: 65,
                    width: 250,
                    child: Padding(
                      padding: const EdgeInsets.all(8.0),
                      child: Text(
                        'Place Holder',
                        textAlign: TextAlign.center,
                        style: TextStyle(
                          fontSize: 20,
                          fontWeight: FontWeight.w600,
                          color: Colors.red,
                        ),
                      ),
                    ),
                  ),
                  Container(
                    height: 65,
                    width: 250,
                    child: Padding(
                      padding: const EdgeInsets.all(8.0),
                      child: Text(
                        'Place Holder',
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
            color: Theme.of(context).primaryColor,
            alignment: Alignment.center,
            child: Padding(
              padding: const EdgeInsets.symmetric(horizontal: 15.0),
              child: Row(
                crossAxisAlignment: CrossAxisAlignment.center,
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  // Text('Driver 1', style: TextStyle(fontWeight: FontWeight.normal, fontSize: 14)),
                  Row(
                    crossAxisAlignment: CrossAxisAlignment.center,
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Padding(
                        padding: const EdgeInsets.only(right: 10.0),
                        child: Image.asset('assets/icons/driver_25.png', filterQuality: FilterQuality.high, height: 18, width: 18),
                      ),
                      Text('Driver Name', style: TextStyle(fontWeight: FontWeight.normal, fontSize: 14)),
                    ],
                  ),
                  // Text('00:08:32', style: TextStyle(fontWeight: FontWeight.normal, fontSize: 14)),
                  Row(
                    crossAxisAlignment: CrossAxisAlignment.center,
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Text('00:08:32', style: TextStyle(fontWeight: FontWeight.normal, fontSize: 14)),
                      Padding(
                        padding: const EdgeInsets.only(left: 10.0),
                        child: Icon(Icons.access_time, color: Colors.black, size: 18),
                      ),
                    ],
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
