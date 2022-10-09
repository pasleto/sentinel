import 'package:fluent_ui/fluent_ui.dart';
import 'package:provider/provider.dart';

import '../providers/theme_provider.dart';
import '../responsive.dart';
import '../widgets/appbar.dart';

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
      // mouseCursor: SystemMouseCursors.click,
      title: const Text('Home'),
      body: const Center(child: Text('Home Page')),
    ),
    // PaneItemHeader(
    //   header: const Text('Test'),
    // ),
    PaneItemSeparator(),

    // PaneItemExpander(
    //   icon: const Icon(FluentIcons.account_management),
    //   title: const Text('Account'),
    //   body: const SizedBox.shrink(),
    //   items: [
    //     PaneItem(
    //       icon: const Icon(FluentIcons.mail),
    //       title: const Text('Mail'),
    //       body: const Center(child: Text('Mail Page')),
    //     ),
    //     PaneItem(
    //       icon: const Icon(FluentIcons.calendar),
    //       title: const Text('Calendar'),
    //       body: const Center(child: Text('Calendar Page')),
    //     ),
    //   ],
    // ),
    PaneItem(
      icon: const Icon(FluentIcons.accounts),
      // mouseCursor: SystemMouseCursors.click,
      title: const Text('Accounts'),
      body: const Center(child: Text('Accounts Page')),
    ),
    PaneItem(
      icon: const Icon(FluentIcons.album),
      // mouseCursor: SystemMouseCursors.click,
      title: const Text('Album'),
      body: const Center(child: Text('Album Page')),
    ),
    PaneItem(
      icon: const Icon(FluentIcons.folder),
      // mouseCursor: SystemMouseCursors.click,
      title: const Text('Folder'),
      body: const Center(child: Text('Folder Page')),
    ),
    PaneItem(
      icon: const Icon(FluentIcons.tiles),
      // mouseCursor: SystemMouseCursors.click,
      title: const Text('Tiles'),
      body: const Center(child: Text('Tiles Page')),
    ),
  ];
  final List<NavigationPaneItem> footerItems = [
    PaneItemSeparator(),
    PaneItem(
      icon: const Icon(FluentIcons.settings),
      // mouseCursor: SystemMouseCursors.click,
      title: const Text('Settings'),
      body: const Center(child: Text('Settings Page')),
      // trailing: const Icon(FluentIcons.settings),
      // infoBadge: const Icon(FluentIcons.settings),
    ),
  ];
  late List<NavigationPaneItem> items = originalItems;

  @override
  void initState() {
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
    final appTheme = context.watch<AppTheme>();
    return SafeArea(
      child: NavigationView(
        key: viewKey,
        appBar: appBarMain(),
        pane: NavigationPane(
          size: const NavigationPaneSize(
            compactWidth: 46,
            // openMaxWidth: 192,
          ),
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
          // header: const SizedBox(
          //   height: kOneLineTileHeight,
          //   child: Center(child: Text('ABC')),
          // ),
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
                // prefix: const SizedBox(width: 2.0),
                suffix: const Padding(
                  padding: EdgeInsets.only(right: 8.0, left: 2.0),
                  child: Icon(FluentIcons.search),
                ),
              ),
            ),
          ),
          displayMode: appTheme.displayMode,
          indicator: () {
            switch (appTheme.indicator) {
              case NavigationIndicators.end:
                return const EndNavigationIndicator();
              case NavigationIndicators.sticky:
              default:
                return const StickyNavigationIndicator();
            }
          }(),
          items: items,
          footerItems: searchValue.isNotEmpty ? [] : footerItems,
        ),
        // onOpenSearch: () {
        //   searchFocusNode.requestFocus();
        // },
      ),
    );
  }
}
