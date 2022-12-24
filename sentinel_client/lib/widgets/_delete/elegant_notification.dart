import 'dart:async';

import 'package:flutter/material.dart';
// import 'package:fluent_ui/fluent_ui.dart';

// ? - testing for custom notifications

const Color greyColor = Color(0xffF3F3F3);

enum AnimationType { fromLeft, fromRight, fromTop, fromBottom }

enum NotificationPosition { center, centerRight, centerLeft, topCenter, topRight, topLeft, bottomCenter, bottomRight, bottomLeft }

extension NotificationPositionExtension on NotificationPosition {
  Alignment get alignment {
    switch (this) {
      case NotificationPosition.center:
        return Alignment.center;
      case NotificationPosition.centerRight:
        return Alignment.centerRight;
      case NotificationPosition.centerLeft:
        return Alignment.centerLeft;
      case NotificationPosition.topCenter:
        return Alignment.topCenter;
      case NotificationPosition.topRight:
        return Alignment.topRight;
      case NotificationPosition.topLeft:
        return Alignment.topLeft;
      case NotificationPosition.bottomCenter:
        return Alignment.bottomCenter;
      case NotificationPosition.bottomRight:
        return Alignment.bottomRight;
      case NotificationPosition.bottomLeft:
        return Alignment.bottomLeft;
      default:
        return Alignment.topCenter;
    }
  }
}

class ToastContent extends StatelessWidget {
  const ToastContent({
    super.key,
    this.title,
    required this.description,
    required this.displayCloseButton,
    this.icon,
    this.onCloseButtonPressed,
    this.iconSize = 20,
    this.action,
    this.onActionPressed,
  });

  /// The title of the notification if any
  final Text? title;

  /// The description of the notification text string
  final Text description;

  /// The notification icon
  final Widget? icon;

  /// The icon size on pixels
  final double iconSize;

  /// Display or hide the close button widget
  final bool displayCloseButton;

  /// The function invoked when pressing the close button
  final Function()? onCloseButtonPressed;

  /// Action widget rendered with clickable inkwell, by default `action == null`
  final Widget? action;

  /// Function invoked when pressing `action` widget, must be not null when `action != null`
  final Function()? onActionPressed;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Padding(
          padding: const EdgeInsets.only(left: 10),
          child: icon!,
        ),
        const SizedBox(width: 15),
        Padding(
          padding: const EdgeInsets.only(top: 20, bottom: 20),
          child: Container(width: 1, color: greyColor),
        ),
        const SizedBox(width: 15),
        Expanded(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              if (title != null) ...[
                title!,
                const SizedBox(height: 5),
              ],
              description,
              if (action != null) ...[
                const SizedBox(height: 5),
                onActionPressed == null
                    ? action!
                    : InkWell(
                        onTap: onActionPressed,
                        child: action!,
                      )
              ]
            ],
          ),
        ),
        Visibility(
          visible: displayCloseButton,
          child: InkWell(
            onTap: () {
              Navigator.pop(context);
              onCloseButtonPressed?.call();
            },
            child: Padding(
              padding: const EdgeInsets.only(top: 20, right: 10),
              child: Column(
                mainAxisAlignment: MainAxisAlignment.start,
                children: const [
                  Icon(Icons.close, color: Colors.grey, size: 15),
                ],
              ),
            ),
          ),
        )
      ],
    );
  }

  // Widget _getNotificationIcon() {
  //   return icon!;
  // }
}

// ignore: must_be_immutable
class AnimatedProgressBar extends StatefulWidget {
  final Color foregroundColor;
  final Duration duration;
  double value = 0;

  AnimatedProgressBar({super.key, required this.foregroundColor, required this.duration});

  @override
  _AnimatedProgressBarState createState() {
    return _AnimatedProgressBarState();
  }
}

class _AnimatedProgressBarState extends State<AnimatedProgressBar> with SingleTickerProviderStateMixin {
  late AnimationController _controller;
  late Animation<double> curve;
  late Tween<double> valueTween;

  @override
  void initState() {
    super.initState();

    _controller = AnimationController(
      duration: widget.duration,
      vsync: this,
    );
    curve = CurvedAnimation(
      parent: _controller,
      curve: Curves.easeInOut,
    );
    valueTween = Tween<double>(
      begin: 1,
      end: 0,
    );
    _controller.forward();
  }

  @override
  void didUpdateWidget(AnimatedProgressBar oldWidget) {
    super.didUpdateWidget(oldWidget);

    if (widget.value != oldWidget.value) {
      double beginValue = valueTween.evaluate(curve);
      valueTween = Tween<double>(
        begin: beginValue,
        end: widget.value,
      );

      _controller
        ..value = 0
        ..forward();
    }
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return AnimatedBuilder(
      animation: curve,
      builder: (context, child) {
        return LinearProgressIndicator(
          backgroundColor: greyColor,
          valueColor: AlwaysStoppedAnimation(widget.foregroundColor),
          value: valueTween.evaluate(curve),
        );
      },
    );
  }
}

// ignore: must_be_immutable
class ElegantNotification extends StatefulWidget {
  ElegantNotification({
    super.key,
    this.title,
    required this.description,
    required this.icon,
    this.shadowColor = Colors.grey,
    this.background = Colors.white,
    this.radius = 5.0,
    this.enableShadow = true,
    this.showProgressIndicator = true,
    this.displayCloseButton = true,
    this.progressIndicatorColor = Colors.blue,
    this.toastDuration = const Duration(milliseconds: 3000),
    this.onCloseButtonPressed,
    this.onProgressFinished,
    this.notificationPosition = NotificationPosition.topRight,
    this.animation = AnimationType.fromRight,
    this.animationDuration = const Duration(milliseconds: 600),
    this.iconSize = 20,
    this.action,
    this.onActionPressed,
    this.autoDismiss = true,
    this.height,
    this.width,
    this.onDismiss,
  }) {
    if (showProgressIndicator) {
      assert(autoDismiss != false);
    }
    if (action != null) {
      assert(onActionPressed != null);
    }

    if (notificationPosition == NotificationPosition.centerRight) {
      assert(animation != AnimationType.fromLeft && animation != AnimationType.fromBottom && animation != AnimationType.fromTop);
    } else if (notificationPosition == NotificationPosition.centerLeft) {
      assert(animation != AnimationType.fromRight && animation != AnimationType.fromBottom && animation != AnimationType.fromTop);
    } else if (notificationPosition == NotificationPosition.topCenter) {
      assert(animation != AnimationType.fromBottom && animation != AnimationType.fromLeft && animation != AnimationType.fromRight);
    } else if (notificationPosition == NotificationPosition.topRight) {
      assert(animation != AnimationType.fromLeft && animation != AnimationType.fromBottom);
    } else if (notificationPosition == NotificationPosition.topLeft) {
      assert(animation != AnimationType.fromRight && animation != AnimationType.fromBottom);
    } else if (notificationPosition == NotificationPosition.bottomCenter) {
      assert(animation != AnimationType.fromTop && animation != AnimationType.fromLeft && animation != AnimationType.fromRight);
    } else if (notificationPosition == NotificationPosition.bottomRight) {
      assert(animation != AnimationType.fromLeft && animation != AnimationType.fromTop);
    } else if (notificationPosition == NotificationPosition.bottomLeft) {
      assert(animation != AnimationType.fromRight && animation != AnimationType.fromTop);
    }
  }

  /// The toast title if any
  final Text? title;

  /// The toast description text (required)
  final Text description;

  /// The toast icon, required only if using the default constructor
  late Widget? icon;

  /// The size of the icon, by default it's 40px
  late double iconSize;

  /// The type of the animation set on the notification, possible values { fromLeft, fromRight, fromTop, fromBottom }, default value `fromLeft`
  final AnimationType animation;

  /// The duration of the animation, Default value `Duration(milliseconds: 600)`
  final Duration animationDuration;

  /// The shadow color applied on the notification widget, by defualt it's `Colors.grey`
  late Color shadowColor = Colors.grey;

  /// the background color of the notification, by default it's set to white
  late Color background;

  /// The color of the progress, by default it's blue
  late Color progressIndicatorColor;

  /// the border radius of the notification widget
  late double radius = 5.0;

  /// How much the notification will take time, by default the duration is `3000 milliseconds`
  final Duration toastDuration;

  /// enable or disable the shadow rendering, by default it's true
  late bool enableShadow = true;

  /// enable or disable the progress indicator rendering, by default the indicator is displayed
  late bool showProgressIndicator;

  /// Display or hide the close button, by default the close button is displayed
  final bool displayCloseButton;

  /// Function invoked when user press on the close button
  final Function()? onCloseButtonPressed;

  /// Function invoked when the notification is closed after the finish of the progress indicator
  final Function()? onProgressFinished;

  /// The type of the align set on the notification, possible values { top, center, bottom }, default value `top`
  final NotificationPosition notificationPosition;

  /// Action widget rendered with clickable inkwell, by default `action == null`
  final Widget? action;

  /// Function invoked when pressing `action` widget, must be not null when `action != null`
  final Function()? onActionPressed;

  /// define whether the notification will be dismissed automatically or not, by default `autoDimiss == false`
  final bool autoDismiss;

  /// the width of the notification widget
  final double? width;

  /// the height of the notification widget
  final double? height;

  /// Function invoked when tapping outside the notification or when pressing the back button of the phone or when tapping on the screen
  final Function()? onDismiss;

  /// display the notification on the screen [context] the context of the application
  void show(BuildContext context) {
    // Navigator.of(context).push(
    //   PageRouteBuilder<Widget>(
    //     fullscreenDialog: false,
    //     pageBuilder: (BuildContext context, _, __) => GestureDetector(
    //       onTap: onDismiss != null
    //           ? () {
    //               Navigator.pop(context);
    //               // TODO - slidercontroller.reverse here
    //               onDismiss!.call();
    //             }
    //           : null,
    //       child: WillPopScope(
    //         onWillPop: () async {
    //           // TODO - slidercontroller.reverse here
    //           onDismiss?.call();
    //           return true;
    //         },
    //         child: SafeArea(
    //           child: AlertDialog(
    //             alignment: notificationPosition.alignment,
    //             backgroundColor: Colors.transparent,
    //             contentPadding: const EdgeInsets.all(0),
    //             insetPadding: const EdgeInsets.all(30),
    //             elevation: 0,
    //             content: this,
    //           ),
    //         ),
    //       ),
    //     ),
    //     opaque: false,
    //     barrierDismissible: true, // TODO
    //   ),
    // );
    Navigator.of(context).push(
      PageRouteBuilder<Widget>(
        fullscreenDialog: false,
        opaque: false,
        barrierDismissible: false,
        pageBuilder: (context, _, __) => Stack(
          clipBehavior: Clip.none,
          children: [
            SafeArea(
              child: GestureDetector(
                onTap: onDismiss != null
                    ? () {
                        Navigator.pop(context);
                        // TODO - slidercontroller.reverse here
                        onDismiss!.call();
                      }
                    : null,
                child: AlertDialog(
                  alignment: notificationPosition.alignment,
                  backgroundColor: Colors.transparent,
                  contentPadding: const EdgeInsets.all(0),
                  insetPadding: const EdgeInsets.all(30),
                  elevation: 0,
                  content: this,
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }

  @override
  _ElegantNotificationState createState() => _ElegantNotificationState();
}

class _ElegantNotificationState extends State<ElegantNotification> with SingleTickerProviderStateMixin {
  late Timer closeTimer;
  late Animation<Offset> offsetAnimation;
  late AnimationController slideController;

  @override
  void initState() {
    super.initState();

    closeTimer = Timer(widget.toastDuration, () {
      slideController.reverse();
      slideController.addListener(() {
        if (slideController.isDismissed) {
          Navigator.pop(context);
          widget.onProgressFinished?.call();
        }
      });
    });
    if (!widget.autoDismiss) {
      closeTimer.cancel();
    }
    _initializeAnimation();
  }

  void _initializeAnimation() {
    slideController = AnimationController(duration: widget.animationDuration, vsync: this);

    switch (widget.animation) {
      case AnimationType.fromLeft:
        offsetAnimation = Tween<Offset>(
          begin: const Offset(-2, 0),
          end: const Offset(0, 0),
        ).animate(CurvedAnimation(parent: slideController, curve: Curves.ease));
        break;
      case AnimationType.fromRight:
        offsetAnimation = Tween<Offset>(
          begin: const Offset(2, 0),
          end: const Offset(0, 0),
        ).animate(CurvedAnimation(parent: slideController, curve: Curves.ease));
        break;
      case AnimationType.fromTop:
        offsetAnimation = Tween<Offset>(
          begin: const Offset(0, -7),
          end: const Offset(0, 0),
        ).animate(CurvedAnimation(parent: slideController, curve: Curves.ease));
        break;
      case AnimationType.fromBottom:
        offsetAnimation = Tween<Offset>(
          begin: const Offset(0, 4),
          end: const Offset(0, 0),
        ).animate(CurvedAnimation(parent: slideController, curve: Curves.ease));
        break;
      default:
    }

    /// ! To support Flutter < 3.0.0
    /// This allows a value of type T or T? to be treated as a value of type T?.
    /// We use this so that APIs that have become non-nullable can still be used with `!` and `?` to support older versions of the API as well.

    T? _ambiguate<T>(T? value) => value;
    _ambiguate(WidgetsBinding.instance)?.addPostFrameCallback(
      (_) => slideController.forward(),
    );
  }

  @override
  Widget build(BuildContext context) {
    return SlideTransition(
      position: offsetAnimation,
      child: Container(
        width: widget.width ?? MediaQuery.of(context).size.width * 0.7,
        height: widget.height ?? MediaQuery.of(context).size.height * 0.12,
        decoration: BoxDecoration(
          borderRadius: BorderRadius.circular(widget.radius),
          color: widget.background,
          boxShadow: widget.enableShadow
              ? [
                  BoxShadow(
                    color: widget.shadowColor.withOpacity(0.2),
                    spreadRadius: 1,
                    blurRadius: 1,
                    offset: const Offset(0, 1), // changes position of shadow
                  ),
                ]
              : null,
        ),
        child: Column(
          children: [
            Expanded(
              child: ToastContent(
                title: widget.title,
                description: widget.description,
                displayCloseButton: widget.displayCloseButton,
                icon: widget.icon,
                onCloseButtonPressed: () {
                  closeTimer.cancel();
                  // slideController.reverse();
                  widget.onDismiss?.call();
                },
                iconSize: widget.iconSize,
                action: widget.action,
                onActionPressed: widget.onActionPressed,
              ),
            ),
            if (widget.showProgressIndicator)
              AnimatedProgressBar(
                foregroundColor: widget.progressIndicatorColor,
                duration: widget.toastDuration,
              ),
          ],
        ),
      ),
    );
  }

  @override
  void dispose() {
    slideController.dispose();
    closeTimer.cancel();
    super.dispose();
  }
}
