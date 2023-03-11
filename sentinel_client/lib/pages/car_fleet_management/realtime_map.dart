import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/models/cfCar.model.dart';
import 'package:sentinel_client/models/cfGpsPosition.model.dart';
import 'package:sentinel_client/widgets/page.dart';
import 'package:sentinel_client/widgets/toasts.dart';
import 'package:flutter_map/flutter_map.dart';
import 'package:latlong2/latlong.dart';

import 'package:sentinel_client/services/socket_service.dart';
// import 'package:socket_io_client/socket_io_client.dart';

import 'package:visibility_detector/visibility_detector.dart';

class CarFleetManagementRealtimeMapPage extends StatefulWidget {
  const CarFleetManagementRealtimeMapPage({super.key});

  @override
  State<CarFleetManagementRealtimeMapPage> createState() => _CarFleetManagementRealtimeMapPageState();
}

class _CarFleetManagementRealtimeMapPageState extends State<CarFleetManagementRealtimeMapPage> with PageMixin {
  bool _pageInScope = false;

  bool _initialized = false;
  bool _fetchingFailed = false;
  List<CfCar> _cars = []; // TODO
  DateTime? _lastRefreshTime; // TODO

  final _controllerMap = MapController();

  dynamic gpsStatusEventHandler(dynamic data) {
    print('SOCKET ON: car-fleet-status');
    print(data);
  }

  dynamic gpsRealtimeEventHandler(dynamic data) {
    print('SOCKET ON: car-fleet-realtime');
    // print('car: ${data['car']}');
    print('time: ${data['time']}'); // ? - visualize -> last communcation time
    DateTime time = DateTime.fromMillisecondsSinceEpoch(data['time']);
    print('temp_int: ${data['temp_int']}'); // ? - visualize -> last internal tracker temperature
    print('gps_signal: ${data['gps_signal']}'); // ? - visualize -> last state of gps signal
    // print('gps: ${data['gps']}');

    // if (data['gps_signal']) {
    CfCar? car = _cars.where((element) => element.id == data['car']).first;
    car.gpsPosition = CfGpsPosition.fromJson(data['gps']);

    car.vinNumber = 'Time: $time'; // TODO - remove

    // TODO - maybe switch logic -> have array of gpsPositions with reference to a CfCar

    setState(() {});
    // }
  }

  @override
  void setState(fn) {
    if (mounted) {
      super.setState(fn);
    }
  }

  @override
  void initState() {
    super.initState();

    print('carfleet-management-realtime-map initState');

    WidgetsBinding.instance.addPostFrameCallback((_) {
      // Use `MapController` as needed
      // SocketService.socket.on('car-fleet-status', gpsStatusEventHandler);
      // SocketService.socket.on('car-fleet-realtime', gpsRealtimeEventHandler);
    });

    // SocketService.socket.on('car-fleet-status', gpsStatusEventHandler);
    // SocketService.socket.on('car-fleet-realtime', gpsRealtimeEventHandler);
  }

  @override
  void dispose() {
    // SocketService.socket.off('car-fleet-status', gpsStatusEventHandler);
    // SocketService.socket.off('car-fleet-realtime', gpsRealtimeEventHandler);
    // print('DISPOSE');
    // SocketService.socket.clearListeners();

    print('carfleet-management-realtime-map dispose');

    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return VisibilityDetector(
      key: const Key('carfleet-management-realtime-map'),
      onVisibilityChanged: (visibilityInfo) {
        if (visibilityInfo.visibleFraction == 1) {
          if (!_pageInScope) {
            setState(() => _pageInScope = !_pageInScope);

            print('carfleet-management-realtime-map loading');

            SocketService.socket.emitWithAck('cf-car-get-all', {}, ack: (response) {
              if (response['status'] == 'OK') {
                Iterable incoming = response['data']['cars'];
                setState(() {
                  _cars = List<CfCar>.from(incoming.map((model) => CfCar.fromJson(model)));
                  print(incoming); // TODO
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

            // WidgetsBinding.instance.addPostFrameCallback((_) {
            // Use `MapController` as needed
            SocketService.socket.on('car-fleet-status', gpsStatusEventHandler);
            SocketService.socket.on('car-fleet-realtime', gpsRealtimeEventHandler);
            // });
          }
        }
        if (visibilityInfo.visibleFraction == 0) {
          if (_pageInScope && mounted) {
            setState(() => _pageInScope = !_pageInScope);

            print('carfleet-management-realtime-map unloading');
            SocketService.socket.off('car-fleet-status', gpsStatusEventHandler);
            SocketService.socket.off('car-fleet-realtime', gpsRealtimeEventHandler);
          }
        }
      },
      child: ScaffoldPage(
        header: PageHeader(
          title: const Text('Car Fleet - Realtime Map', style: TextStyle(fontSize: 18.0, fontWeight: FontWeight.w600)),
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
                    message: "Last refresh",
                    child: w,
                  ),
                  wrappedItem: CommandBarButton(
                    icon: SizedBox(
                      height: 16,
                      width: 56,
                      child: _lastRefreshTime != null
                          ? Text('${_lastRefreshTime?.toLocal().hour.toString()}:${_lastRefreshTime?.toLocal().minute.toString()}')
                          : const Text('N/A'),
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
                    child: FlutterMap(
                      mapController: _controllerMap,
                      options: MapOptions(
                        center: LatLng(50.582820, 16.326122), // TODO - varibale inside database - homeBase gps coordinades - default to null island
                        zoom: 15.0, // TODO - settings inside database - slider from 0 to 18 - defaults to 9 ?
                        maxZoom: 18.0,
                        // minZoom: 2.0,
                        keepAlive: true,
                        interactiveFlags: InteractiveFlag.all & ~InteractiveFlag.rotate & ~InteractiveFlag.doubleTapZoom /*& ~InteractiveFlag.flingAnimation*/,
                        maxBounds: LatLngBounds(LatLng(-90, -180.0), LatLng(90.0, 180.0)),
                      ),
                      nonRotatedChildren: [
                        AttributionWidget(
                          attributionBuilder: (context) {
                            return const Align(
                              alignment: Alignment.bottomRight,
                              child: ColoredBox(
                                color: Color(0x55FFFFFF),
                                child: Padding(
                                  padding: EdgeInsets.all(3),
                                  child: Text('© OpenStreetMap', style: TextStyle(color: Color(0xCC000000), fontSize: 10.0)),
                                ),
                              ),
                            );
                          },
                        ),
                      ],
                      children: [
                        // TODO - constants, maybe enroll own tile server
                        // https://switch2osm.org/serving-tiles/
                        // https://download.geofabrik.de/
                        // http://leaflet-extras.github.io/leaflet-providers/preview/
                        TileLayer(
                          urlTemplate: 'https://tile.openstreetmap.org/{z}/{x}/{y}.png',
                          // urlTemplate: 'https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',
                          // subdomains: const ['a', 'b', 'c'],
                          userAgentPackageName: 'org.pasler.sentinel.client',
                          // tileBuilder: darkModeTileBuilder,
                          // tilesContainerBuilder: darkModeTilesContainerBuilder,
                          // tilesContainerBuilder: (context, tilesContainer, tiles) {
                          //   // TODO
                          //   return ColorFiltered(
                          //     colorFilter: ColorFilter.matrix(
                          //       FluentTheme.of(context).brightness.isDark
                          //           ? <double>[
                          //               // darker
                          //               1, 0, 0, 0, -128,
                          //               0, 1, 0, 0, -128,
                          //               0, 0, 1, 0, -128,
                          //               0, 0, 0, 1, 0,
                          //             ]
                          //           : <double>[
                          //               // identity
                          //               1, 0, 0, 0, 0,
                          //               0, 1, 0, 0, 0,
                          //               0, 0, 1, 0, 0,
                          //               0, 0, 0, 1, 0,
                          //             ],
                          //     ),
                          //     child: tilesContainer,
                          //   );
                          // },
                          backgroundColor: Colors.transparent,
                        ),
                        MarkerLayer(
                          markers: List<Marker>.generate(_cars.where((element) => element.gpsPosition != null).toList().length, (index) {
                            CfCar car = _cars.where((element) => element.gpsPosition != null).toList()[index];
                            return Marker(
                              point: LatLng(car.gpsPosition!.lat.toDouble(), car.gpsPosition!.lon.toDouble()),
                              height: 10,
                              width: 10,
                              builder: (context) {
                                return Flyout(
                                  openMode: FlyoutOpenMode.press,
                                  placement: FlyoutPlacement.center,
                                  position: FlyoutPosition.below,
                                  content: (context) {
                                    return FlyoutContent(
                                      // child: Text('Flyout Here'),
                                      child: Column(
                                        crossAxisAlignment: CrossAxisAlignment.center,
                                        mainAxisAlignment: MainAxisAlignment.center,
                                        mainAxisSize: MainAxisSize.min,
                                        children: [
                                          Text('Lat: ${car.gpsPosition!.lat}'),
                                          Text('Lon: ${car.gpsPosition!.lon}'),
                                          Text('Alt: ${car.gpsPosition!.alt}'),
                                          Text('Speed: ${car.gpsPosition!.speed}'),
                                          Text('Accuracy: ${car.gpsPosition!.accuracy}'),
                                          Text('Time: ${car.gpsPosition!.time}'),
                                        ],
                                      ),
                                    );
                                  },
                                  child: Container(
                                    decoration: BoxDecoration(
                                      shape: BoxShape.circle,
                                      // color: FluentTheme.of(context).menuColor,
                                      // border: Border.all(color: FluentTheme.of(context).accentColor, width: 2.0),
                                      color: FluentTheme.of(context).accentColor,
                                    ),
                                    // child: const Padding(
                                    //   padding: EdgeInsets.only(bottom: 4.0),
                                    //   child: Icon(FluentIcons.car, size: 14),
                                    // ),
                                  ),
                                );
                              },
                            );
                          }),
                        ),
                      ],
                    ),
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
