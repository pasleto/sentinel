import 'package:fluent_ui/fluent_ui.dart';

class Responsive extends StatelessWidget {
  final Widget mobile;
  final Widget? tablet;
  final Widget desktop;

  const Responsive({
    super.key,
    required this.mobile,
    this.tablet,
    required this.desktop,
  }); // const Responsive({Key? key, required this.mobile, this.tablet, required this.desktop}) : super(key: key);

  static bool isMobile(BuildContext context) => MediaQuery.of(context).size.width < 641;

  static bool isTablet(BuildContext context) => MediaQuery.of(context).size.width < 1008 && MediaQuery.of(context).size.width >= 641;

  static bool isDesktop(BuildContext context) => MediaQuery.of(context).size.width >= 1008;

  @override
  Widget build(BuildContext context) {
    final Size screenSize = MediaQuery.of(context).size;
    if (screenSize.width >= 1008) {
      return desktop;
    } else if (screenSize.width >= 641 && tablet != null) {
      return tablet!;
    } else {
      return mobile;
    }
  }
}
