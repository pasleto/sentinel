import 'package:fluent_ui/fluent_ui.dart';

abstract class DisposableProvider with ChangeNotifier {
  void disposeValues();
}
