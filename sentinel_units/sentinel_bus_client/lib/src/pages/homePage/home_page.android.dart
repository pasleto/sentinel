import 'package:flutter/material.dart';

class AndroidHomePage extends StatefulWidget {
  AndroidHomePage({Key? key}) : super(key: key);

  @override
  _AndroidHomePageState createState() => _AndroidHomePageState();
}

class _AndroidHomePageState extends State<AndroidHomePage> {
  _handleSyncAvailability() {
    // TODO - handle some stuff
    return true;
  }

  _handleSync() {
    print('Sync Application Data');
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
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.center,
                mainAxisAlignment: MainAxisAlignment.spaceAround,
                children: [
                  TextButton(
                    onPressed: () => Navigator.pushNamed(context, '/drive-type'),
                    onLongPress: null,
                    style: TextButton.styleFrom(backgroundColor: Colors.grey[200]),
                    child: Container(
                      height: MediaQuery.of(context).size.height * 0.35,
                      width: MediaQuery.of(context).size.width * 0.80,
                      child: Center(
                        child: Text('START', style: TextStyle(color: Colors.black, fontWeight: FontWeight.normal, fontSize: 64), textAlign: TextAlign.center),
                      ),
                    ),
                  ),
                  // Container(
                  //   height: MediaQuery.of(context).size.height * 0.03,
                  //   width: MediaQuery.of(context).size.width * 0.80,
                  // ),
                  TextButton(
                    // TODO - bool for disabling button
                    onPressed: this._handleSyncAvailability() ? () => this._handleSync() : null,
                    onLongPress: null,
                    style: TextButton.styleFrom(
                      backgroundColor: this._handleSyncAvailability() ? Colors.grey[200] : Colors.redAccent[100],
                    ),
                    child: Container(
                      height: MediaQuery.of(context).size.height * 0.15,
                      width: MediaQuery.of(context).size.width * 0.80,
                      child: Center(
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.center,
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            // Padding(padding: const EdgeInsets.only(bottom: 10.0), child: Icon(Icons.sync_alt, color: Colors.black, size: 36)),
                            Icon(Icons.sync_alt, color: Colors.black, size: 30),
                            Padding(
                              padding: const EdgeInsets.symmetric(vertical: 8.0),
                              child:
                                  Text('SYNC', style: TextStyle(color: Colors.black, fontWeight: FontWeight.normal, fontSize: 26), textAlign: TextAlign.center),
                            ),
                            Text('Not synchronized rides:  0',
                                style: TextStyle(color: Colors.black, fontWeight: FontWeight.normal, fontSize: 12), textAlign: TextAlign.center),
                          ],
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
            // color: Colors.red,
            alignment: Alignment.center,
            // child: Padding(
            //   padding: const EdgeInsets.symmetric(horizontal: 15.0),
            child: Row(
              crossAxisAlignment: CrossAxisAlignment.center,
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Container(
                  height: double.infinity,
                  width: MediaQuery.of(context).size.width * 0.40,
                  color: Colors.red,
                  child: Row(
                    crossAxisAlignment: CrossAxisAlignment.center,
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      Text('Wi-Fi: ', style: TextStyle(fontWeight: FontWeight.w600, fontSize: 13)),
                      Text('NaN', style: TextStyle(fontWeight: FontWeight.w600, fontSize: 13)),
                    ],
                  ),
                ),
                Container(
                  height: double.infinity,
                  width: MediaQuery.of(context).size.width * 0.20,
                  color: Colors.green,
                  child: TextButton(onPressed: () => print('reload button'), child: Icon(Icons.restart_alt, color: Colors.black)),
                ),
                Container(
                  height: double.infinity,
                  width: MediaQuery.of(context).size.width * 0.40,
                  color: Colors.blue,
                  child: Row(
                    crossAxisAlignment: CrossAxisAlignment.center,
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      Text('Server: ', style: TextStyle(fontWeight: FontWeight.w600, fontSize: 13)),
                      Text('NaN', style: TextStyle(fontWeight: FontWeight.w600, fontSize: 13)),
                    ],
                  ),
                ),
                // Row(
                //   crossAxisAlignment: CrossAxisAlignment.center,
                //   mainAxisAlignment: MainAxisAlignment.spaceBetween,
                //   children: [
                //     Text('WiFi: ', style: TextStyle(fontWeight: FontWeight.w600, fontSize: 13)),
                //     Text('Connected', style: TextStyle(fontWeight: FontWeight.w600, fontSize: 13)),
                //   ],
                // ),
                // TextButton(onPressed: () => print('reload button'), child: Icon(Icons.restart_alt, color: Colors.black)),
                // Row(
                //   crossAxisAlignment: CrossAxisAlignment.center,
                //   mainAxisAlignment: MainAxisAlignment.spaceBetween,
                //   children: [
                //     Text('Server: ', style: TextStyle(fontWeight: FontWeight.w600, fontSize: 13)),
                //     Text('Online', style: TextStyle(fontWeight: FontWeight.w600, fontSize: 13)),
                //   ],
                // ),
              ],
            ),
          ),
          // ),
        ],
      ),
    );
  }
}
