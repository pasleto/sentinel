import 'package:fluent_ui/fluent_ui.dart';
import 'package:provider/provider.dart';
import 'package:sentinel_client/providers/app_providers.dart';
import 'package:sentinel_client/providers/auth_token_provider.dart';
import 'package:sentinel_client/services/socket_service.dart';
import 'package:sentinel_client/providers/theme_provider.dart';
import 'package:sentinel_client/providers/user_provider.dart';
import 'package:sentinel_client/widgets/toasts.dart';

// import 'package:sentinel_client/routes/screens/root.dart' as root_screens;

class DarkModeSwitch extends StatelessWidget {
  const DarkModeSwitch({super.key});

  @override
  Widget build(BuildContext context) {
    final appTheme = context.watch<AppTheme>();

    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 4.0),
      child: Tooltip(
        displayHorizontally: false,
        useMousePosition: false,
        style: const TooltipThemeData(showDuration: Duration(milliseconds: 500), waitDuration: Duration(milliseconds: 500), preferBelow: true),
        message: () {
          switch (appTheme.mode) {
            case ThemeMode.system:
              return 'System mode on';
            case ThemeMode.light:
              return 'Light mode on';
            case ThemeMode.dark:
              return 'Dark mode on';
          }
        }(),
        child: IconButton(
          icon: () {
            switch (appTheme.mode) {
              case ThemeMode.system:
                return const Icon(FluentIcons.system, size: 18);
              case ThemeMode.light:
                return const Icon(FluentIcons.sunny, size: 18);
              case ThemeMode.dark:
                return const Icon(FluentIcons.clear_night, size: 18);
            }
          }(),
          onPressed: () {
            switch (appTheme.mode) {
              case ThemeMode.system:
                appTheme.setModeToStorage('light');
                break;
              case ThemeMode.light:
                appTheme.setModeToStorage('dark');
                break;
              case ThemeMode.dark:
                appTheme.setModeToStorage('system');
                break;
            }
          },
        ),
      ),
      // ),
    );
  }
}

class LogoutButton extends StatelessWidget {
  const LogoutButton({super.key});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 4.0),
      child: IconButton(
        icon: const Icon(FluentIcons.sign_out, size: 18),
        onPressed: () {
          // TODO - sent logout to backend
          AppToasts.dismissAll(atSameTime: true);
          Provider.of<AuthToken>(context, listen: false).deleteFromStorage();
          SocketService.disconnect();
          SocketService.dispose();
          AppProviders.disposeAllDisposableProviders(context);
        },
      ),
      // ),
    );
  }
}

class NotificationsManager extends StatelessWidget {
  const NotificationsManager({super.key});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 4.0),
      child: Stack(
        clipBehavior: Clip.none,
        children: [
          IconButton(
            icon: const Icon(FluentIcons.ringer, size: 18),
            onPressed: () {
              // AppToasts.generic(
              //   context: context,
              //   // duration: const Duration(seconds: 5),
              //   closeButton: true,
              //   trailingAction: const Text('trailing'),
              // );
              // AppToasts.generic(
              //   context: context,
              //   titleText: 'Generic',
              //   // messageText: 'Notification Message Text',
              //   // duration: const Duration(seconds: 5),
              //   closeButton: true,
              //   leadingIcon: const Icon(FluentIcons.user_sync, size: 20),
              //   // trailingAction: const Text('trailing'),
              // );
              // AppToasts.generic(
              //   context: context,
              //   titleText: 'Generic',
              //   messageText: 'Notification Message Text which is pretty long and wont fit',
              //   // duration: const Duration(seconds: 5),
              //   closeButton: true,
              //   leadingIcon: const Icon(FluentIcons.user_sync, size: 20),
              //   // trailingAction: const Text('trailing'),
              // );

              AppToasts.genericNotification(
                context: context,
                titleText: 'Generic',
                messageText: 'Notification Message Text',
                // duration: const Duration(seconds: 5),
                closeButton: true,
                leadingIcon: const Icon(FluentIcons.user_sync, size: 20),
                // trailingAction: const Text('trailing'),
              );

              AppToasts.infoNotification(
                context: context,
                messageText: 'Notification Message Text',
                // duration: const Duration(seconds: 5),
                closeButton: true,
                // trailingAction: const Text('trailing'),
              );
              AppToasts.successNotification(
                context: context,
                messageText: 'Notification Message Text',
                // duration: const Duration(seconds: 5),
                closeButton: true,
                // trailingAction: const Text('trailing'),
              );
              AppToasts.warningNotification(
                context: context,
                messageText: 'Notification Message Text',
                // duration: const Duration(seconds: 5),
                closeButton: true,
                // trailingAction: const Text('trailing'),
              );
              AppToasts.errorNotification(
                context: context,
                messageText: 'Notification Message Text',
                // duration: const Duration(seconds: 5),
                closeButton: true,
                // trailingAction: const Text('trailing'),
              );

              debugPrint('notification window request');
            },
          ), // TODO - this show only if new notification exists
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
    );
  }
}

class AccountManager extends StatelessWidget {
  const AccountManager({super.key});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 8.0),
      child: DropDownButton(
        verticalOffset: 30,
        placement: FlyoutPlacement.end,
        closeAfterClick: false,
        leading: Padding(
          padding: const EdgeInsets.only(top: 1.0, bottom: 1.0, right: 8.0),
          child: Consumer<AppUser>(
            builder: (context, appUser, child) {
              if (appUser.isRefreshing) {
                return const SizedBox(
                  width: 20,
                  height: 20,
                  child: Padding(
                    padding: EdgeInsets.all(5.0),
                    child: ProgressRing(strokeWidth: 2.0),
                  ),
                );
              } else {
                return Container(
                  width: 20,
                  height: 20,
                  decoration: BoxDecoration(
                    shape: BoxShape.circle,
                    image: DecorationImage(
                      // image: MemoryImage(appUser.getImageData()),
                      image: MemoryImage(appUser.user.getImageData()),
                      // image: Image.memory(
                      //   appUser.getImageData(),
                      //   frameBuilder: (context, child, frame, wasSynchronouslyLoaded) {
                      //     if (wasSynchronouslyLoaded) return child;
                      //     return AnimatedSwitcher(
                      //       duration: const Duration(milliseconds: 200),
                      //       child: frame != null ? child : const ProgressRing(strokeWidth: 2.0),
                      //     );
                      //   },
                      // ).image,
                      fit: BoxFit.fill,
                      filterQuality: FilterQuality.high,
                    ),
                  ),
                );
              }
            },
          ),
        ),
        items: [
          MenuFlyoutItem(
            leading: Padding(
              padding: const EdgeInsets.only(top: 6.0, bottom: 6.0, left: 8.0, right: 4.0),
              child: Consumer<AppUser>(
                builder: (context, appUser, child) {
                  if (appUser.isRefreshing) {
                    return const SizedBox(
                      width: 28,
                      height: 28,
                      child: Padding(
                        padding: EdgeInsets.all(7.0),
                        child: ProgressRing(strokeWidth: 4),
                      ),
                    );
                  } else {
                    return Container(
                      width: 28,
                      height: 28,
                      decoration: BoxDecoration(
                        shape: BoxShape.circle,
                        image: DecorationImage(
                          // image: MemoryImage(appUser.getImageData()),
                          image: MemoryImage(appUser.user.getImageData()),
                          fit: BoxFit.fill,
                          filterQuality: FilterQuality.high,
                        ),
                      ),
                    );
                  }
                },
              ),
            ),
            text: SizedBox(
              width: 72, // ?
              child: Padding(
                padding: const EdgeInsets.only(top: 4.0, bottom: 4.0),
                child: Consumer<AppUser>(
                  builder: (context, appUser, child) {
                    return Text(
                      appUser.user.username,
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
            onPressed: () {
              // TODO - sent logout to backend
              AppToasts.dismissAll(atSameTime: true);
              Provider.of<AuthToken>(context, listen: false).deleteFromStorage();
              SocketService.disconnect();
              SocketService.dispose();
              AppProviders.disposeAllDisposableProviders(context);
            },
          ),
        ],
      ),
    );
  }
}
