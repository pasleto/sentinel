import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/widgets/appbar.dart';

class LoaderScreen extends StatelessWidget {
  const LoaderScreen({super.key, this.message, this.showAppbar = false, this.showLogoutButton = false});

  final String? message;
  final bool showAppbar;
  final bool showLogoutButton;

  @override
  Widget build(BuildContext context) {
    if (message != null) {
      return SafeArea(
        child: NavigationView(
          transitionBuilder: (child, animation) {
            return DrillInPageTransition(animation: animation, child: child);
          },
          appBar: showAppbar ? appBarLoader(showLogoutButton) : null,
          content: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              Expanded(
                child: Center(
                  child: Padding(
                    padding: showAppbar ? const EdgeInsets.only(top: 14.0) : const EdgeInsets.only(top: 64.0),
                    child: const ProgressRing(),
                  ),
                ),
              ),
              Padding(
                padding: const EdgeInsets.only(bottom: 64.0, left: 32.0, right: 32.0),
                child: Text(
                  message!,
                  textAlign: TextAlign.center,
                ),
              ),
            ],
          ),
        ),
      );
    } else {
      return SafeArea(
        child: NavigationView(
          transitionBuilder: (child, animation) {
            return DrillInPageTransition(animation: animation, child: child);
          },
          appBar: showAppbar ? appBarLoader(showLogoutButton) : null,
          content: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              Expanded(
                child: Center(
                  child: Padding(
                    padding: showAppbar ? const EdgeInsets.only(bottom: 64.0) : const EdgeInsets.only(bottom: 14.0),
                    child: const ProgressRing(),
                  ),
                ),
              ),
            ],
          ),
        ),
      );
    }
  }
}
