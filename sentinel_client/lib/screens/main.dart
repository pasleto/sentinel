import 'package:fluent_ui/fluent_ui.dart';
import 'package:provider/provider.dart';

import 'package:sentinel_client/providers/theme_provider.dart';
// import 'package:sentinel_client/responsive.dart';
// import 'package:sentinel_client/providers/user_provider.dart';
// import '../responsive.dart';
import 'package:sentinel_client/widgets/appbar.dart';
import 'package:sentinel_client/utils/deferred_widget.dart';

// import 'package:sentinel_client/widgets/page.dart';

import 'package:sentinel_client/routes/pages/root.dart' as root_pages;
import 'package:sentinel_client/routes/pages/access_control_management.dart' deferred as access_control_management;
import 'package:sentinel_client/routes/pages/user_management.dart' deferred as user_management;
import 'package:sentinel_client/routes/pages/material_storage_management.dart' deferred as material_storage_management;
import 'package:sentinel_client/routes/pages/car_fleet_management.dart' deferred as car_fleet_management;

class MainScreen extends StatefulWidget {
  const MainScreen({super.key});

  @override
  State<MainScreen> createState() => _MainScreenState();
}

class _MainScreenState extends State<MainScreen> {
  int index = 0;
  final viewKey = GlobalKey();
  final searchKey = GlobalKey();
  final searchFocusNode = FocusNode();
  final searchController = TextEditingController();
  void resetSearch() => searchController.clear();

  String get searchValue => searchController.text;

  final List<NavigationPaneItem> originalItems = [
    // PaneItemSeparator(),
    // PaneItemSeparator(thickness: 0.0),
    PaneItem(
      icon: const Icon(FluentIcons.home),
      title: const Text('Home'),
      body: const root_pages.HomePage(),
    ),
    PaneItemSeparator(),
    // PaneItemExpander(
    _NoBodyPaneItemExpander(
      icon: const Icon(FluentIcons.service_activity),
      title: const Text('IT Service Management'),
      body: const SizedBox.shrink(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.service_off),
          title: const Text('Incidents'),
          body: const SizedBox.shrink(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.branch_pull_request),
          title: const Text('Change Requests'),
          body: const SizedBox.shrink(),
        ),
      ],
    ),
    PaneItemSeparator(),
    // PaneItemExpander(
    _NoBodyPaneItemExpander(
      icon: const Icon(FluentIcons.account_management),
      title: const Text('User Management'),
      // body: DeferredWidget(
      //   user_management.loadLibrary,
      //   () => user_management.UserManagementDashboardPage(),
      // ),
      body: const SizedBox.shrink(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.view_dashboard),
          title: const Text('Dashboard'),
          body: DeferredWidget(
            user_management.loadLibrary,
            () => user_management.UserManagementDashboardPage(),
          ),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.external_user),
          title: const Text('Departments'),
          body: DeferredWidget(
            user_management.loadLibrary,
            () => user_management.UserManagementDepartmentsPage(),
          ),
        ),
      ],
    ),
    // PaneItemExpander(
    _NoBodyPaneItemExpander(
      icon: const Icon(FluentIcons.customer_assets),
      title: const Text('Material Storage Management'),
      // body: DeferredWidget(
      //   material_storage_management.loadLibrary,
      //   () => material_storage_management.MaterialStorageManagementDashboardPage(),
      // ),
      body: const SizedBox.shrink(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.view_dashboard),
          title: const Text('Dashboard'),
          body: DeferredWidget(
            material_storage_management.loadLibrary,
            () => material_storage_management.MaterialStorageManagementDashboardPage(),
          ),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.master_database),
          // icon: const Icon(FluentIcons.data_management_settings),
          title: const Text('Master Data'),
          body: const SizedBox.shrink(),
        ),
        PaneItem(
          // icon: const Icon(FluentIcons.product_catalog),
          icon: const Icon(FluentIcons.factory),
          title: const Text('Vendors'),
          body: const SizedBox.shrink(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.product),
          title: const Text('Products'),
          body: const SizedBox.shrink(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.product_list),
          title: const Text('Materials'),
          body: const SizedBox.shrink(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.delivery_truck),
          title: const Text('Suppliers'),
          body: const SizedBox.shrink(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.product_variant),
          title: const Text('Supplies'),
          body: const SizedBox.shrink(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.engineering_group),
          title: const Text('Groups'),
          body: const SizedBox.shrink(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.tablet),
          title: const Text('Devices'),
          body: DeferredWidget(
            material_storage_management.loadLibrary,
            () => material_storage_management.MaterialStorageManagementDevicesPage(),
          ),
        ),
      ],
    ),
    // PaneItemExpander(
    _NoBodyPaneItemExpander(
      icon: const Icon(FluentIcons.contact_card_settings),
      title: const Text('Access Control Management'),
      // body: DeferredWidget(
      //   access_control_management.loadLibrary,
      //   () => access_control_management.AccessControlManagementDashboardPage(),
      // ),
      body: const SizedBox.shrink(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.view_dashboard),
          title: const Text('Dashboard'),
          body: DeferredWidget(
            access_control_management.loadLibrary,
            () => access_control_management.AccessControlManagementDashboardPage(),
          ),
        ),
      ],
    ),
    // PaneItemExpander(
    _NoBodyPaneItemExpander(
      icon: const Icon(FluentIcons.fixed_asset_management),
      title: const Text('IT Asset Management'),
      body: const SizedBox.shrink(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.view_dashboard),
          title: const Text('Dashboard'),
          body: const SizedBox.shrink(),
        ),
      ],
    ),
    // PaneItemExpander(
    _NoBodyPaneItemExpander(
      icon: const Icon(FluentIcons.car),
      title: const Text('Car Fleet Management'),
      body: const SizedBox.shrink(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.view_dashboard),
          title: const Text('Dashboard'),
          body: DeferredWidget(
            car_fleet_management.loadLibrary,
            () => car_fleet_management.CarFleetManagementDashboardPage(),
          ),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.car),
          title: const Text('Cars'),
          body: const SizedBox.shrink(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.map_pin),
          title: const Text('Realtime Map'),
          body: DeferredWidget(
            car_fleet_management.loadLibrary,
            () => car_fleet_management.CarFleetManagementRealtimeMapPage(),
          ),
        ),
      ],
    ),
    // PaneItemExpander(
    _NoBodyPaneItemExpander(
      icon: const Icon(FluentIcons.certificate),
      title: const Text('License Management'),
      body: const SizedBox.shrink(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.view_dashboard),
          title: const Text('Dashboard'),
          body: const SizedBox.shrink(),
        ),
      ],
    ),
  ];
  final List<NavigationPaneItem> footerItems = [
    PaneItemSeparator(),
    PaneItem(
      icon: const Icon(FluentIcons.settings),
      title: const Text('Settings'),
      body: const root_pages.SettingsPage(),
    ),
    _TextPaneItem(
      icon: const Icon(FluentIcons.open_source, size: 14),
      title: const Text(
        'Powered by Sentinel',
        maxLines: 1,
        textAlign: TextAlign.center,
        style: TextStyle(
          fontStyle: FontStyle.italic,
          fontSize: 10,
          fontWeight: FontWeight.w100,
          color: Color.fromARGB(192, 128, 128, 128),
          height: 1.0,
        ),
      ),
      body: const SizedBox.shrink(),
    ),
  ];
  late List<NavigationPaneItem> items = originalItems;

  @override
  void initState() {
    DeferredWidget.preload(access_control_management.loadLibrary);
    DeferredWidget.preload(user_management.loadLibrary);
    DeferredWidget.preload(material_storage_management.loadLibrary);

    searchController.addListener(() {
      setState(() {
        if (searchValue.isEmpty) {
          items = originalItems;
        } else {
          items = [...originalItems, ...footerItems]
              .whereType<PaneItem>()
              .where((item) {
                assert(item.title is Text);
                final text = (item.title as Text).data!;
                return text.toLowerCase().contains(searchValue.toLowerCase());
              })
              .toList()
              .cast<NavigationPaneItem>();
        }
      });
    });
    super.initState();
  }

  @override
  void dispose() {
    searchController.dispose();
    searchFocusNode.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    // final appTheme = context.watch<AppTheme>();
    return SafeArea(
      // child: NavigationView(
      child: Consumer<AppTheme>(
        builder: (context, appTheme, child) {
          return NavigationView(
            key: viewKey,
            appBar: appBarMain(),
            transitionBuilder: (child, animation) {
              return DrillInPageTransition(animation: animation, child: child);
            },
            pane: NavigationPane(
              size: const NavigationPaneSize(
                compactWidth: 46,
                // openMaxWidth: 192,
              ),
              // menuButton: const SizedBox.shrink(),
              // menuButton: (appTheme.displayMode == PaneDisplayMode.compact || appTheme.displayMode == PaneDisplayMode.open)
              //     ? SizedBox(
              //         height: 43,
              //         width: 46,
              //         child: Padding(
              //           padding: const EdgeInsets.only(left: 6.0, right: 6.0, bottom: 6.0, top: 3.0),
              //           child: IconButton(
              //             icon: const Icon(FluentIcons.global_nav_button),
              //             onPressed: () {
              //               switch (appTheme.displayMode) {
              //                 case PaneDisplayMode.compact:
              //                   appTheme.displayMode = PaneDisplayMode.open;
              //                   break;
              //                 case PaneDisplayMode.open:
              //                 default:
              //                   appTheme.displayMode = PaneDisplayMode.compact;
              //                   break;
              //               }
              //             },
              //           ),
              //         ),
              //       )
              //     : null,
              // selected: index,
              // onChanged: (i) => setState(() => index = i),
              // header: const SizedBox(
              //   height: kOneLineTileHeight,
              //   child: Center(child: Text('ABC')),
              // ),
              selected: () {
                if (searchValue.isEmpty) return index;
                final indexOnScreen = items.indexOf([...originalItems, ...footerItems].whereType<PaneItem>().elementAt(index));
                if (indexOnScreen.isNegative) return null;
                return indexOnScreen;
              }(),
              onChanged: (i) {
                if (searchValue.isNotEmpty) {
                  final equivalentIndex = [...originalItems, ...footerItems].whereType<PaneItem>().toList().indexOf(items[i] as PaneItem);
                  i = equivalentIndex;
                }
                resetSearch();
                setState(() => index = i);
              },
              header: SizedBox(
                height: kOneLineTileHeight,
                child: Padding(
                  padding: const EdgeInsets.only(left: 6.0, right: 6.0, bottom: 5.0, top: 1.0),
                  child: TextFormBox(
                    minHeight: 40,
                    key: searchKey,
                    textAlignVertical: TextAlignVertical.center,
                    controller: searchController,
                    placeholder: 'Search',
                    focusNode: searchFocusNode,
                    suffix: const Padding(
                      padding: EdgeInsets.only(right: 6.0, left: 2.0),
                      child: Icon(FluentIcons.search),
                    ),
                  ),
                ),
              ),
              // displayMode: () {
              //   WidgetsBinding.instance.addPostFrameCallback((_) {
              //     if (Responsive.isMobile(context)) {
              //       if (appTheme.displayMode != PaneDisplayMode.minimal) {
              //         appTheme.displayMode = PaneDisplayMode.minimal;
              //       }
              //     } else {
              //       if (appTheme.displayMode != PaneDisplayMode.compact) {
              //         appTheme.displayMode = PaneDisplayMode.compact;
              //       }
              //     }
              //   });
              //   return appTheme.displayMode;
              // }(),
              displayMode: appTheme.displayMode,
              // displayMode: () {
              //   WidgetsBinding.instance.addPostFrameCallback((_) {
              //     if (Responsive.isMobile(context)) {
              //       if (appTheme.displayMode != PaneDisplayMode.minimal) {
              //         appTheme.displayMode = PaneDisplayMode.minimal;
              //       }
              //     } else if (Responsive.isTablet(context)) {
              //       if (appTheme.displayMode != PaneDisplayMode.compact) {
              //         appTheme.displayMode = PaneDisplayMode.compact;
              //       }
              //     } else if (Responsive.isDesktop(context)) {
              //       if (appTheme.displayMode != PaneDisplayMode.open) {
              //         appTheme.displayMode = PaneDisplayMode.open;
              //       }
              //     }
              //   });
              //   return appTheme.displayMode;
              // }(),
              indicator: () {
                switch (appTheme.indicator) {
                  case NavigationIndicators.end:
                    return const EndNavigationIndicator();
                  case NavigationIndicators.sticky:
                  default:
                    // return const StickyNavigationIndicator();
                    return const StickyNavigationIndicator(
                      leftPadding: 5.0,
                      curve: Curves.easeInOut,
                      duration: Duration(milliseconds: 250),
                    );
                }
              }(),
              items: items,
              footerItems: searchValue.isNotEmpty ? [] : footerItems,
            ),
          );
        },
      ),
    );
  }
}

class _TextPaneItem extends PaneItem {
  _TextPaneItem({
    required super.icon,
    required super.body,
    super.title,
    // super.onTap,
  });

  @override
  Widget build(
    BuildContext context,
    bool selected,
    VoidCallback? onPressed, {
    PaneDisplayMode? displayMode,
    bool showTextOnTop = true,
    bool? autofocus,
    int? itemIndex,
  }) {
    return Builder(
      builder: (context) => super.build(
        context,
        selected,
        () {},
        displayMode: displayMode,
        showTextOnTop: showTextOnTop,
        itemIndex: itemIndex,
        autofocus: autofocus,
      ),
    );
  }
}

class _NoBodyPaneItemExpander extends PaneItemExpander {
  // ! error -> setState() called after dispose(): _StickyNavigationIndicatorState#55d5a(lifecycle state: defunct, not mounted, tickers: tracking 0 tickers)
  // ! when using compact displayMode
  _NoBodyPaneItemExpander({
    required super.icon,
    required super.body,
    required super.items,
    super.title,
  });

  @override
  Widget build(
    BuildContext context,
    bool selected,
    VoidCallback? onPressed, {
    PaneDisplayMode? displayMode,
    bool showTextOnTop = true,
    ValueChanged<PaneItem>? onItemPressed,
    bool? autofocus,
    int? itemIndex,
  }) {
    return Builder(
      builder: (context) => super.build(
        context,
        selected,
        () {},
        displayMode: displayMode,
        showTextOnTop: showTextOnTop,
        onItemPressed: onItemPressed,
        autofocus: autofocus,
        itemIndex: itemIndex,
      ),
    );
  }
}
