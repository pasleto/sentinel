import 'package:fluent_ui/fluent_ui.dart';
import 'appbar_actions.dart';

NavigationAppBar appBarLogin() {
  return NavigationAppBar(
    automaticallyImplyLeading: false,
    // title: Center(
    //   child: Row(
    //     mainAxisAlignment: MainAxisAlignment.start,
    //     crossAxisAlignment: CrossAxisAlignment.center,
    //     children: [
    //       Padding(
    //         padding: const EdgeInsets.only(right: 16.0, bottom: 1.5),
    //         child: Image.asset('assets/logo/Logo_32.png', scale: 1.4, filterQuality: FilterQuality.high),
    //       ),
    //     ],
    //   ),
    // ),
    actions: Center(
      child: Padding(
        padding: const EdgeInsets.only(right: 12.0),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.end,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: const [
            DarkModeSwitch(),
          ],
        ),
      ),
    ),
  );
}

NavigationAppBar appBarMain() {
  return NavigationAppBar(
    automaticallyImplyLeading: false,
    title: Center(
      child: Row(
        mainAxisAlignment: MainAxisAlignment.start,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          Padding(
            padding: const EdgeInsets.only(right: 16.0, bottom: 1.5),
            child: Image.asset('assets/logo/Logo_32.png', scale: 1.4, filterQuality: FilterQuality.high),
          ),
          const Text('Sentinel', style: TextStyle(fontSize: 21)),
        ],
      ),
    ),
    actions: Center(
      child: Padding(
        padding: const EdgeInsets.only(right: 8.0),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.end,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: const [
            DarkModeSwitch(),
            Notifications(),
            AccountManager(),
          ],
        ),
      ),
    ),
  );
}
