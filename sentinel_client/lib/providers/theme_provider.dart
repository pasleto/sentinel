import 'package:fluent_ui/fluent_ui.dart';
import 'package:flutter/foundation.dart';
import 'package:system_theme/system_theme.dart';
import 'package:flutter_acrylic/flutter_acrylic.dart';

import 'package:sentinel_client/providers/disposable_provider.dart';
import 'package:sentinel_client/services/storage_service.dart';

enum NavigationIndicators { sticky, end }

class AppTheme extends DisposableProvider {
  AccentColor _color = systemAccentColor;
  AccentColor get color => _color;
  set color(AccentColor color) {
    _color = color;
    notifyListeners();
  }

  ThemeMode _mode = ThemeMode.system;
  ThemeMode get mode => _mode;
  set mode(ThemeMode mode) {
    _mode = mode;
    notifyListeners();
  }

  void initMode() async {
    String? savedThemeMode = await StorageService.readSecureData('themeMode');
    if (savedThemeMode != null) {
      switch (savedThemeMode) {
        case 'system':
          _mode = ThemeMode.system;
          break;
        case 'light':
          _mode = ThemeMode.light;
          break;
        case 'dark':
          _mode = ThemeMode.dark;
          break;
      }
    } else {
      _mode = ThemeMode.system;
    }
    notifyListeners();
  }

  void setModeToStorage(String mode) async {
    switch (mode) {
      case 'system':
        await StorageService.writeSecureData('themeMode', 'system');
        _mode = ThemeMode.system;
        break;
      case 'light':
        await StorageService.writeSecureData('themeMode', 'light');
        _mode = ThemeMode.light;
        break;
      case 'dark':
        await StorageService.writeSecureData('themeMode', 'dark');
        _mode = ThemeMode.dark;
        break;
    }
    notifyListeners();

    // await StorageService.writeSecureData('themeMode', mode);
  }

  void deleteModeFromStorage() async {
    await StorageService.deleteSecureData('themeMode');
  }

  // PaneDisplayMode _displayMode = PaneDisplayMode.compact;
  PaneDisplayMode _displayMode = PaneDisplayMode.auto;
  PaneDisplayMode get displayMode => _displayMode;
  set displayMode(PaneDisplayMode displayMode) {
    _displayMode = displayMode;
    notifyListeners();
  }

  NavigationIndicators _indicator = NavigationIndicators.sticky;
  NavigationIndicators get indicator => _indicator;
  set indicator(NavigationIndicators indicator) {
    _indicator = indicator;
    notifyListeners();
  }

  WindowEffect _windowEffect = WindowEffect.disabled;
  WindowEffect get windowEffect => _windowEffect;
  set windowEffect(WindowEffect windowEffect) {
    _windowEffect = windowEffect;
    notifyListeners();
  }

  void setEffect(WindowEffect effect, BuildContext context) {
    Window.setEffect(
      effect: effect,
      color: [
        WindowEffect.solid,
        WindowEffect.acrylic,
      ].contains(effect)
          ? FluentTheme.of(context).micaBackgroundColor.withOpacity(0.05)
          : Colors.transparent,
      dark: FluentTheme.of(context).brightness.isDark,
    );
  }

  TextDirection _textDirection = TextDirection.ltr;
  TextDirection get textDirection => _textDirection;
  set textDirection(TextDirection direction) {
    _textDirection = direction;
    notifyListeners();
  }

  // Locale? _locale = const Locale('cs');
  Locale? _locale;
  Locale? get locale => _locale;
  set locale(Locale? locale) {
    _locale = locale;
    notifyListeners();
  }

  @override
  void disposeValues() {
    // ? implement disposeValues
    // _color = systemAccentColor;
    // _mode = ThemeMode.system;
    // _displayMode = PaneDisplayMode.auto;
    // _indicator = NavigationIndicators.sticky;
    // _windowEffect = WindowEffect.disabled;
    // _textDirection = TextDirection.ltr;
    // notifyListeners();
  }
}

AccentColor get systemAccentColor {
  if ((defaultTargetPlatform == TargetPlatform.windows || defaultTargetPlatform == TargetPlatform.android) && !kIsWeb) {
    // if (defaultTargetPlatform == TargetPlatform.windows || defaultTargetPlatform == TargetPlatform.android || kIsWeb) {
    debugPrint('ACCENT COLOR: ${SystemTheme.accentColor.accent.toString()}');
    return AccentColor.swatch({
      'darkest': SystemTheme.accentColor.darkest,
      'darker': SystemTheme.accentColor.darker,
      'dark': SystemTheme.accentColor.dark,
      'normal': SystemTheme.accentColor.accent,
      'light': SystemTheme.accentColor.light,
      'lighter': SystemTheme.accentColor.lighter,
      'lightest': SystemTheme.accentColor.lightest,
    });
  }
  return Colors.blue;
}
