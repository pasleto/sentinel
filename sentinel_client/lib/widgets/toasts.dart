import 'package:fluent_ui/fluent_ui.dart';
import 'package:overlayment/overlayment.dart';
import 'package:sentinel_client/screens/loader.dart';

class NotificationBody extends StatelessWidget {
  // TODO - progress timer bar
  const NotificationBody({super.key, this.leadingIcon, this.titleText, this.messageText, this.trailing, this.closeButton = true, this.notificationName});

  final Icon? leadingIcon;
  final Widget? trailing;
  final bool closeButton;
  final String? titleText;
  final String? messageText;
  final String? notificationName;

  final double toastMinWidth = 75;
  final double toastMaxWidth = 225;

  @override
  Widget build(BuildContext context) {
    String name = notificationName ?? 'Notification$hashCode';

    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 8.0),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.end,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          if (leadingIcon != null)
            Row(
              children: [
                Padding(
                  padding: const EdgeInsets.symmetric(horizontal: 6.0),
                  child: Padding(
                    padding: const EdgeInsets.only(left: 8.0),
                    child: leadingIcon,
                  ),
                ),
                Padding(
                  padding: const EdgeInsets.only(left: 6.0, top: 2.0, bottom: 2.0),
                  child: Container(width: 2, color: const Color.fromARGB(100, 175, 175, 175)),
                ),
              ],
            ),
          if (titleText != null || messageText != null)
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 12.0),
              child: Padding(
                padding: EdgeInsets.only(left: leadingIcon != null ? 0.0 : 6.0, right: closeButton ? 0.0 : 6.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    if (titleText != null)
                      Padding(
                        padding: const EdgeInsets.symmetric(vertical: 3.0),
                        child: Container(
                          constraints: BoxConstraints(maxWidth: toastMaxWidth, minWidth: toastMinWidth),
                          child: Text(titleText!, style: const TextStyle(fontWeight: FontWeight.w600, height: 1.3)),
                        ),
                      ),
                    if (messageText != null)
                      Padding(
                        padding: const EdgeInsets.symmetric(vertical: 3.0),
                        child: Container(
                          constraints: BoxConstraints(maxWidth: toastMaxWidth, minWidth: toastMinWidth),
                          child: Text(messageText!, style: const TextStyle(height: 1.3)),
                        ),
                      ),
                  ],
                ),
              ),
            ),
          if (trailing != null)
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 6.0),
              child: Padding(
                padding: EdgeInsets.only(left: (titleText != null || messageText != null) ? 6.0 : 12.0, right: closeButton ? 0.0 : 12.0),
                child: trailing,
              ),
            ),
          if (closeButton)
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 12.0),
              child: IconButton(
                icon: const Icon(FluentIcons.clear),
                onPressed: () => Overlayment.dismissName(name),
              ),
            ),
        ],
      ),
    );
  }
}

class AppToasts {
  static void dismissAll({bool atSameTime = false}) {
    Overlayment.dismissAll(atSameTime: atSameTime);
  }

  static void dismissLast() {
    Overlayment.dismissLast();
  }

  static void dismissNamed(String name) {
    Overlayment.dismissName(name);
  }

  static void genericNotification({
    required BuildContext context,
    String? titleText,
    String? messageText,
    bool closeButton = true,
    Duration? duration,
    Widget? trailingAction,
    Icon? leadingIcon,
    String? name,
  }) {
    Overlayment.show(
      OverNotification(
        child: NotificationBody(
          leadingIcon: leadingIcon,
          titleText: titleText,
          messageText: messageText,
          closeButton: closeButton,
          trailing: trailingAction,
          notificationName: name,
        ),
        alignment: Alignment.bottomRight, // TODO - response
        duration: duration,
        offset: const Offset(-10.0, -10.0),
      ),
      context: context,
    );
  }

  static void infoNotification({
    required BuildContext context,
    String titleText = 'Info',
    required String messageText,
    bool closeButton = true,
    Duration? duration,
    Widget? trailingAction,
    String? name,
  }) {
    Overlayment.show(
      OverNotification(
        child: NotificationBody(
          leadingIcon: const Icon(FluentIcons.info, size: 20),
          titleText: titleText,
          messageText: messageText,
          closeButton: closeButton,
          trailing: trailingAction,
          notificationName: name,
        ),
        alignment: Alignment.bottomRight, // TODO - response
        duration: duration,
        offset: const Offset(-10.0, -10.0),
        color: const Color.fromARGB(190, 30, 185, 255), // warning
      ),
      context: context,
    );
  }

  static void successNotification({
    required BuildContext context,
    String titleText = 'Success',
    required String messageText,
    bool closeButton = true,
    Duration? duration,
    Widget? trailingAction,
    String? name,
  }) {
    Overlayment.show(
      OverNotification(
        child: NotificationBody(
          leadingIcon: const Icon(FluentIcons.accept, size: 18),
          titleText: titleText,
          messageText: messageText,
          closeButton: closeButton,
          trailing: trailingAction,
          notificationName: name,
        ),
        alignment: Alignment.bottomRight, // TODO - response
        duration: duration,
        offset: const Offset(-10.0, -10.0),
        color: const Color.fromARGB(190, 15, 190, 15), // success
      ),
      context: context,
    );
  }

  static void warningNotification({
    required BuildContext context,
    String titleText = 'Warning',
    required String messageText,
    bool closeButton = true,
    Duration? duration,
    Widget? trailingAction,
    String? name,
  }) {
    Overlayment.show(
      OverNotification(
        child: NotificationBody(
          leadingIcon: const Icon(FluentIcons.warning, size: 20),
          titleText: titleText,
          messageText: messageText,
          closeButton: closeButton,
          trailing: trailingAction,
          notificationName: name,
        ),
        alignment: Alignment.bottomRight, // TODO - response
        duration: duration,
        offset: const Offset(-10.0, -10.0),
        color: const Color.fromARGB(190, 255, 190, 0), // warning
      ),
      context: context,
    );
  }

  static void errorNotification({
    required BuildContext context,
    String titleText = 'Error',
    required String messageText,
    bool closeButton = true,
    Duration? duration,
    Widget? trailingAction,
    String? name,
  }) {
    Overlayment.show(
      OverNotification(
        child: NotificationBody(
          leadingIcon: const Icon(FluentIcons.error_badge, size: 20),
          titleText: titleText,
          messageText: messageText,
          closeButton: closeButton,
          trailing: trailingAction,
          notificationName: name,
        ),
        alignment: Alignment.bottomRight, // TODO - response
        duration: duration,
        offset: const Offset(-10.0, -10.0),
        color: const Color.fromARGB(190, 255, 10, 10), // error
      ),
      context: context,
    );
  }

  static void fullscreenLoaderPanel({
    required BuildContext context,
    String? messageText,
    String? panelName,
    bool appBar = false,
    bool logoutButton = false,
  }) {
    String name = panelName ?? 'LoaderPanel${context.hashCode}';
    Overlayment.show(
      OverPanel(
        animation: const OverFadeAnimation(),
        name: name,
        child: LoaderScreen(
          message: messageText,
          showAppbar: appBar, // true,
          showLogoutButton: logoutButton, // true,
        ),
        alignment: Alignment.center,
        duration: null,
        backgroundSettings: const BackgroundSettings(
          dismissOnClick: false,
          blur: 1.0,
        ),
        width: double.infinity,
        height: double.infinity,
      ),
      context: context,
    );
  }
}
