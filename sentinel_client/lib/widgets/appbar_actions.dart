import 'package:fluent_ui/fluent_ui.dart';
import 'package:provider/provider.dart';
import '../providers/theme_provider.dart';
import '../providers/user_provider.dart';

class DarkModeSwitch extends StatelessWidget {
  const DarkModeSwitch({super.key});

  @override
  Widget build(BuildContext context) {
    final appTheme = context.watch<AppTheme>();

    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 4.0),
      // child: MouseRegion(
      //   cursor: SystemMouseCursors.click,
      child: IconButton(
        icon: FluentTheme.of(context).brightness.isDark ? const Icon(FluentIcons.clear_night, size: 18) : const Icon(FluentIcons.sunny, size: 18),
        onPressed: () {
          if (FluentTheme.of(context).brightness.isDark) {
            appTheme.mode = ThemeMode.light;
          } else {
            appTheme.mode = ThemeMode.dark;
          }
        },
      ),
      // ),
    );
  }
}

class Notifications extends StatelessWidget {
  const Notifications({super.key});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 4.0),
      // child: MouseRegion(
      //   cursor: SystemMouseCursors.click,
      child: Stack(
        clipBehavior: Clip.none,
        children: [
          IconButton(
            icon: const Icon(FluentIcons.ringer, size: 18),
            onPressed: () {
              debugPrint('notification window request');
            },
          ),
          Positioned(
            right: 7.5,
            top: 6.5,
            child: IgnorePointer(
              ignoring: true,
              child: Container(
                width: 6.5,
                height: 6.5,
                decoration: const BoxDecoration(shape: BoxShape.circle, color: Colors.warningPrimaryColor),
              ),
            ),
          ),
        ],
      ),
      // ),
    );
  }
}

class AccountManager extends StatelessWidget {
  const AccountManager({super.key});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 8.0),
      // child: MouseRegion(
      //   cursor: SystemMouseCursors.click,
      child: DropDownButton(
        verticalOffset: 30,
        placement: FlyoutPlacement.end,
        closeAfterClick: false,
        leading: Padding(
          padding: const EdgeInsets.only(top: 1.0, bottom: 1.0, right: 8.0),
          // child: Icon(FluentIcons.account_management, size: 16),
          child: Container(
            width: 20,
            height: 20,
            decoration: const BoxDecoration(
              shape: BoxShape.circle,
              image: DecorationImage(
                image: AssetImage('assets/images/pasleto.jpg'),
                fit: BoxFit.fill,
                filterQuality: FilterQuality.high,
              ),
            ),
          ),
        ),
        items: [
          MenuFlyoutItem(
            leading: Padding(
              padding: const EdgeInsets.only(top: 6.0, bottom: 6.0, left: 8.0, right: 4.0),
              child: Container(
                width: 28,
                height: 28,
                decoration: const BoxDecoration(
                  shape: BoxShape.circle,
                  image: DecorationImage(
                    image: AssetImage('assets/images/pasleto.jpg'),
                    fit: BoxFit.fill,
                    filterQuality: FilterQuality.high,
                  ),
                ),
              ),
            ),
            text: SizedBox(
              width: 72, // ?
              child: Padding(
                padding: const EdgeInsets.only(top: 4.0, bottom: 4.0),
                child: Consumer<AppUser>(
                  builder: (context, user, child) {
                    return Text(
                      user.username,
                      textAlign: TextAlign.center,
                      style: const TextStyle(height: 1.3),
                    );
                  },
                ),
              ),
            ),
            onPressed: (() {}),
          ),
          MenuFlyoutItem(
            leading: const Padding(
              padding: EdgeInsets.only(top: 3.0, bottom: 2.0, left: 2.0, right: 8.0),
              child: Icon(FluentIcons.password_field),
            ),
            text: const Padding(
              padding: EdgeInsets.only(left: 2.0, top: 2.0, bottom: 3.0),
              child: Text('Change password', textAlign: TextAlign.center),
            ),
            onPressed: (() {
              debugPrint('password change request');
              Navigator.pop(context);
            }),
          ),
          MenuFlyoutItem(
            leading: const Padding(
              padding: EdgeInsets.only(top: 3.0, bottom: 2.0, left: 2.0, right: 8.0),
              child: Icon(FluentIcons.sign_out),
            ),
            text: const Padding(
              padding: EdgeInsets.only(left: 2.0, top: 2.0, bottom: 3.0),
              child: Text('Logout', textAlign: TextAlign.center),
            ),
            onPressed: (() {
              debugPrint('logout request');
              Navigator.pop(context);
            }),
          ),
        ],
      ),
      // ),
    );
  }
}
