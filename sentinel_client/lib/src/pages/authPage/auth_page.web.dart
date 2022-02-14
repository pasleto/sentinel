import 'package:flutter/material.dart';

class WebAuthPage extends StatefulWidget {
  WebAuthPage({Key? key}) : super(key: key);

  @override
  _WebAuthPageState createState() => _WebAuthPageState();
}

class _WebAuthAppBar extends StatelessWidget implements PreferredSizeWidget {
  @override
  Widget build(BuildContext context) {
    return AppBar(
      automaticallyImplyLeading: false,
      titleSpacing: 10.0,
      title: Align(
        alignment: Alignment.centerLeft,
        child: Row(
          children: [
            Padding(
              padding: const EdgeInsets.only(top: 7.0, bottom: 7.0, right: 10.0),
              child: Image.asset('assets/logo/Logo_Black_32.png', height: 18, filterQuality: FilterQuality.high),
            ),
            Padding(
              padding: const EdgeInsets.only(top: 7.0, bottom: 7.0),
              child: Image.asset('assets/logo/Logo_Text_Black_16.png', filterQuality: FilterQuality.high, height: 12),
            ),
          ],
        ),
      ),
    );
  }

  @override
  Size get preferredSize {
    return new Size.fromHeight(30);
  }
}

class _WebAuthPageState extends State<WebAuthPage> {
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: _WebAuthAppBar(),
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
                      padding: const EdgeInsets.all(16.0),
                      child: Image.asset('assets/logo/Logo_128.png', width: 96, height: 96, filterQuality: FilterQuality.high),
                    ),
                    Padding(
                      padding: const EdgeInsets.all(16.0),
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
      ),
    );
  }
}
