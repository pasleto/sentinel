import 'package:fluent_ui/fluent_ui.dart';
import 'package:provider/provider.dart';
import 'package:sentinel_client/providers/constants_provider.dart';
import 'package:sentinel_client/widgets/appbar_actions.dart';

NavigationAppBar appBarLogin() {
  return NavigationAppBar(
    automaticallyImplyLeading: false,
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

NavigationAppBar appBarLoader(bool showLogoutButton) {
  return NavigationAppBar(
    automaticallyImplyLeading: false,
    title: Center(
      child: Row(
        mainAxisAlignment: MainAxisAlignment.start,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          Padding(
            padding: const EdgeInsets.only(right: 12.0, bottom: 2.0),
            child: Image.asset('assets/logo/Logo_32.png', height: 24, width: 24, filterQuality: FilterQuality.high, cacheHeight: 24, cacheWidth: 24),
          ),
          Consumer<AppConstants>(
            builder: (context, constant, child) {
              return Text(constant.appName, style: const TextStyle(fontSize: 21));
            },
          ),
        ],
      ),
    ),
    actions: Center(
      child: Padding(
        padding: const EdgeInsets.only(right: 12.0),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.end,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            const DarkModeSwitch(),
            if (showLogoutButton) const LogoutButton(),
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
            padding: const EdgeInsets.only(right: 12.0, bottom: 2.0),
            child: Image.asset('assets/logo/Logo_32.png', height: 24, width: 24, filterQuality: FilterQuality.high, cacheHeight: 24, cacheWidth: 24),
          ),
          Consumer<AppConstants>(
            builder: (context, constant, child) {
              return Text(constant.appName, style: const TextStyle(fontSize: 21));
            },
          ),
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
            NotificationsManager(),
            AccountManager(),
          ],
        ),
      ),
    ),
  );
}
