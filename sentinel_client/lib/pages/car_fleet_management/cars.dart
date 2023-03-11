import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/models/cfCar.model.dart';
import 'package:sentinel_client/widgets/page.dart';
import 'package:sentinel_client/widgets/toasts.dart';

import 'package:visibility_detector/visibility_detector.dart';

import 'package:sentinel_client/services/socket_service.dart';

class CarFleetManagementCarsPage extends StatefulWidget {
  const CarFleetManagementCarsPage({super.key});

  @override
  State<CarFleetManagementCarsPage> createState() => _CarFleetManagementCarsPageState();
}

class _CarFleetManagementCarsPageState extends State<CarFleetManagementCarsPage> with PageMixin {
  bool _pageInScope = false;

  bool _initialized = false;
  bool _fetchingFailed = false;
  List<CfCar> _cars = []; // TODO

  @override
  void setState(fn) {
    if (mounted) {
      super.setState(fn);
    }
  }

  @override
  void initState() {
    super.initState();

    print('carfleet-management-cars initState');

    WidgetsBinding.instance.addPostFrameCallback((_) {
      // Use `MapController` as needed
    });
  }

  @override
  void dispose() {
    print('carfleet-management-cars dispose');

    // setState(() => _pageInScope = false);

    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return VisibilityDetector(
      key: const Key('carfleet-management-cars'),
      onVisibilityChanged: (visibilityInfo) {
        if (visibilityInfo.visibleFraction == 1) {
          if (!_pageInScope) {
            setState(() => _pageInScope = !_pageInScope);

            print('carfleet-management-cars loading');

            SocketService.socket.emitWithAck('cf-car-get-all', {}, ack: (response) {
              if (response['status'] == 'OK') {
                Iterable incoming = response['data']['cars'];
                setState(() {
                  print(incoming); // TODO

                  _cars = List<CfCar>.from(incoming.map((model) => CfCar.fromJson(model)));

                  _initialized = true;
                  _fetchingFailed = false;
                });
              } else {
                var msg = response['data']['message'];
                AppToasts.errorNotification(
                  context: context,
                  titleText: 'Fetching Data Failed',
                  messageText: msg ?? 'Generic Failure',
                  duration: const Duration(seconds: 5),
                );
                setState(() {
                  _initialized = true;
                  _cars = [];
                  _fetchingFailed = true;
                });
              }
            });
          }
        }
        if (visibilityInfo.visibleFraction == 0) {
          if (_pageInScope && mounted) {
            setState(() => _pageInScope = !_pageInScope);

            print('carfleet-management-cars unloading');
          }
        }
      },
      child: ScaffoldPage(
        header: PageHeader(
          title: const Text('Car Fleet - Cars', style: TextStyle(fontSize: 18.0, fontWeight: FontWeight.w600)),
          // commandBar: CommandBarCard(
          //   padding: const EdgeInsets.symmetric(horizontal: 6.0, vertical: 2.0),
          //   child: CommandBar(
          commandBar: Padding(
            padding: const EdgeInsets.symmetric(vertical: 3.0),
            child: CommandBar(
              mainAxisAlignment: MainAxisAlignment.end,
              overflowBehavior: CommandBarOverflowBehavior.noWrap,
              isCompact: true,
              primaryItems: [
                if (!_initialized)
                  CommandBarBuilderItem(
                    builder: (context, mode, w) => Tooltip(
                      displayHorizontally: false,
                      useMousePosition: false,
                      style: const TooltipThemeData(
                        showDuration: Duration(milliseconds: 500),
                        waitDuration: Duration(milliseconds: 500),
                        preferBelow: true,
                      ),
                      message: "Fetching data",
                      child: w,
                    ),
                    wrappedItem: const CommandBarButton(
                      icon: SizedBox(
                        height: 16,
                        width: 16,
                        child: ProgressRing(strokeWidth: 3.5),
                      ),
                      onPressed: null,
                    ),
                  ),
                if (_fetchingFailed)
                  CommandBarBuilderItem(
                    builder: (context, mode, w) => Tooltip(
                      displayHorizontally: false,
                      useMousePosition: false,
                      style: const TooltipThemeData(
                        showDuration: Duration(milliseconds: 500),
                        waitDuration: Duration(milliseconds: 500),
                        preferBelow: true,
                      ),
                      message: "Fetching data failed",
                      child: w,
                    ),
                    wrappedItem: const CommandBarButton(
                      icon: SizedBox(
                        height: 16,
                        width: 16,
                        child: Icon(FluentIcons.sync_error, color: Colors.warningPrimaryColor),
                      ),
                      onPressed: null,
                    ),
                  ),
                CommandBarBuilderItem(
                  builder: (context, mode, w) => Tooltip(
                    displayHorizontally: false,
                    useMousePosition: false,
                    style: const TooltipThemeData(
                      showDuration: Duration(milliseconds: 500),
                      waitDuration: Duration(milliseconds: 500),
                      preferBelow: true,
                    ),
                    message: "XXX",
                    child: w,
                  ),
                  wrappedItem: const CommandBarButton(
                    icon: SizedBox(
                      height: 16,
                      width: 56,
                      child: Text('N/A'),
                    ),
                    onPressed: null,
                  ),
                ),
              ],
            ),
          ),
        ),
        content: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          mainAxisAlignment: MainAxisAlignment.start,
          mainAxisSize: MainAxisSize.max,
          children: [
            Container(width: double.infinity, height: 1, color: const Color.fromARGB(100, 175, 175, 175)),
            Expanded(
              child: Row(
                crossAxisAlignment: CrossAxisAlignment.start,
                mainAxisAlignment: MainAxisAlignment.start,
                mainAxisSize: MainAxisSize.max,
                children: [
                  Expanded(
                    flex: 1,
                    child: Column(
                      children: [
                        Padding(
                          padding: const EdgeInsets.all(6.0),
                          child: Row(
                            crossAxisAlignment: CrossAxisAlignment.center,
                            mainAxisAlignment: MainAxisAlignment.center,
                            mainAxisSize: MainAxisSize.max,
                            children: [
                              Expanded(
                                child: TextBox(
                                  maxLines: 1,
                                  minLines: 1,
                                  minHeight: 35,
                                  // readOnly: _departments.isEmpty,
                                  // enabled: _departments.isNotEmpty,
                                  textAlignVertical: TextAlignVertical.center,
                                  // controller: _controllerSearch,
                                  autocorrect: false,
                                  keyboardType: TextInputType.text,
                                  // focusNode: _focusNodeSearch,
                                  // onSubmitted: (value) => _search(),
                                  placeholder: ' Licence Plate, VIN Number',
                                  placeholderStyle: const TextStyle(
                                    fontSize: 12,
                                    fontWeight: FontWeight.w300,
                                    fontStyle: FontStyle.italic,
                                  ),
                                  style: const TextStyle(height: 1),
                                  suffix: GestureDetector(
                                    // onTap: _resetSearchWithClear,
                                    child: const Padding(
                                      padding: EdgeInsets.only(right: 6.0, bottom: 1.5),
                                      child: Icon(FluentIcons.status_circle_error_x, size: 20),
                                    ),
                                  ),
                                  suffixMode: OverlayVisibilityMode.editing,
                                ),
                              ),
                              Tooltip(
                                displayHorizontally: false,
                                useMousePosition: false,
                                style: const TooltipThemeData(
                                  showDuration: Duration(milliseconds: 500),
                                  waitDuration: Duration(milliseconds: 500),
                                  preferBelow: true,
                                ),
                                message: "Search",
                                child: Padding(
                                  padding: const EdgeInsets.only(left: 4.0),
                                  child: SizedBox(
                                    height: 34,
                                    child: Button(
                                      // onPressed: _departments.isEmpty ? null : _search, // TODO - when search is running | possible to stop search ?
                                      onPressed: () {},
                                      child: const Padding(
                                        padding: EdgeInsets.only(bottom: 1.0),
                                        child: Icon(FluentIcons.search, size: 14.0), // TODO - when search is running -> spinner
                                      ),
                                    ),
                                  ),
                                ),
                              ),
                            ],
                          ),
                        ),
                        Container(width: double.infinity, height: 1, color: const Color.fromARGB(100, 175, 175, 175)),
                        Expanded(
                          child: Padding(
                            padding: const EdgeInsets.all(4.0),
                            child: _initialized
                                ? _cars.isNotEmpty
                                    ? ListView.builder(
                                        itemCount: _cars.length,
                                        itemBuilder: (context, index) {
                                          final car = _cars[index];
                                          return ListTile.selectable(
                                            onPressed: () {}, // TODO
                                            onSelectionChange: (value) {}, // TODO
                                            selectionMode: ListTileSelectionMode.single, // TODO
                                            selected: false,
                                            title: Text(
                                              car.licencePlateNumber,
                                              maxLines: 1,
                                              overflow: TextOverflow.ellipsis,
                                            ),
                                            subtitle: Padding(
                                              padding: const EdgeInsets.only(top: 4.0),
                                              child: Text(
                                                car.vinNumber,
                                                maxLines: 1,
                                                overflow: TextOverflow.ellipsis,
                                                style: const TextStyle(fontSize: 12, fontWeight: FontWeight.w300, fontStyle: FontStyle.italic),
                                              ),
                                            ),
                                            // trailing: ,
                                          );
                                        },
                                      )
                                    : ListView(
                                        children: const [
                                          ListTile.selectable(
                                            onPressed: null,
                                            onSelectionChange: null,
                                            selectionMode: ListTileSelectionMode.none,
                                            selected: false,
                                            title: Text('No Data', maxLines: 2, overflow: TextOverflow.ellipsis),
                                          ),
                                        ],
                                      )
                                : ListView(
                                    children: [
                                      ListTile.selectable(
                                        onPressed: null,
                                        onSelectionChange: null,
                                        selectionMode: ListTileSelectionMode.none,
                                        selected: false,
                                        title: () {
                                          if (_fetchingFailed) {
                                            return const Text('Data fetch failed!', maxLines: 2, overflow: TextOverflow.ellipsis);
                                          } else {
                                            return const Text('Fetching data ...', maxLines: 2, overflow: TextOverflow.ellipsis);
                                          }
                                        }(),
                                        leading: () {
                                          if (_fetchingFailed) {
                                            return const SizedBox(
                                              width: 20,
                                              height: 20,
                                              child: Icon(FluentIcons.sync_error, size: 14, color: Colors.warningPrimaryColor),
                                            );
                                          } else {
                                            return const SizedBox(
                                              width: 20,
                                              height: 20,
                                              child: Padding(
                                                padding: EdgeInsets.all(4.0),
                                                child: ProgressRing(strokeWidth: 2.0),
                                              ),
                                            );
                                          }
                                        }(),
                                      ),
                                    ],
                                  ),
                          ),
                        ),
                      ],
                    ),
                  ),
                  Expanded(flex: 0, child: Container(width: 1, color: const Color.fromARGB(100, 175, 175, 175))),
                  Expanded(
                    flex: 3,
                    child: Text('ASD'),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
