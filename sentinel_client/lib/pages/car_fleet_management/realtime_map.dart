import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/widgets/page.dart';
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
  // List<CfCars> _cars = []; // TODO - list of cars
  List<String> _cars = ['Car1', 'Car2', 'Car3']; // TODO - list of cars
  DateTime? _lastRefreshTime; // TODO

  final _controllerMap = MapController();

  dynamic gpsStatusEventHandler(dynamic data) {
    print('SOCKET ON: car-fleet-status');
    print(data);
  }

  dynamic gpsRealtimeEventHandler(dynamic data) {
    print('SOCKET ON: car-fleet-realtime');
    print(data);
  }

  @override
  void initState() {
    super.initState();

    print('REALTIME MAP initState');

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

    print('REALTIME MAP dispose');

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

            print('carfleet-realtime-map loading');
            // WidgetsBinding.instance.addPostFrameCallback((_) {
            // Use `MapController` as needed
            SocketService.socket.on('car-fleet-status', gpsStatusEventHandler);
            SocketService.socket.on('car-fleet-realtime', gpsRealtimeEventHandler);
            // });
          }
        }
        if (visibilityInfo.visibleFraction == 0) {
          if (_pageInScope) {
            setState(() => _pageInScope = !_pageInScope);

            print('carfleet-realtime-map unloading');
            SocketService.socket.off('car-fleet-status', gpsStatusEventHandler);
            SocketService.socket.off('car-fleet-realtime', gpsRealtimeEventHandler);
          }
        }
      },
      child: ScaffoldPage(
        header: PageHeader(
          title: const Text('Car Fleet Realtime Map', style: TextStyle(fontSize: 18.0, fontWeight: FontWeight.w600)),
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
                                  placeholder: ' Licence Plate, Model',
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
                            child: ListView.builder(
                              itemCount: _cars.length,
                              itemBuilder: (context, index) {
                                final car = _cars[index];
                                return ListTile.selectable(
                                  onPressed: null,
                                  onSelectionChange: null,
                                  selectionMode: ListTileSelectionMode.none,
                                  selected: false,
                                  title: Text(car),
                                  subtitle: Text('[$car]'),
                                  // trailing: ,
                                );
                              },
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
                          userAgentPackageName: 'org.pasler.sentinel.client',
                          // tileBuilder: darkModeTileBuilder,
                          // tilesContainerBuilder: darkModeTilesContainerBuilder,
                          tilesContainerBuilder: (context, tilesContainer, tiles) {
                            // TODO
                            return ColorFiltered(
                              colorFilter: ColorFilter.matrix(
                                FluentTheme.of(context).brightness.isDark
                                    ? <double>[
                                        // darker
                                        1, 0, 0, 0, -128,
                                        0, 1, 0, 0, -128,
                                        0, 0, 1, 0, -128,
                                        0, 0, 0, 1, 0,
                                      ]
                                    : <double>[
                                        // identity
                                        1, 0, 0, 0, 0,
                                        0, 1, 0, 0, 0,
                                        0, 0, 1, 0, 0,
                                        0, 0, 0, 1, 0,
                                      ],
                              ),
                              child: tilesContainer,
                            );
                          },
                          backgroundColor: Colors.transparent,
                        ),
                        MarkerLayer(
                          markers: [
                            Marker(
                              point: LatLng(50.582792, 16.326170),
                              width: 25,
                              height: 25,
                              // builder: (context) => FlutterLogo(),
                              builder: (context) {
                                return Container(
                                  width: double.maxFinite,
                                  height: double.maxFinite,
                                  alignment: Alignment.center,
                                  decoration: BoxDecoration(
                                    shape: BoxShape.circle,
                                    color: FluentTheme.of(context).menuColor,
                                    border: Border.all(color: FluentTheme.of(context).accentColor, width: 2.0),
                                  ),
                                  child: Flyout(
                                    openMode: FlyoutOpenMode.press,
                                    content: (context) {
                                      return const FlyoutContent(
                                        child: Text('Flyout Here'),
                                      );
                                    },
                                    child: const Padding(
                                      padding: EdgeInsets.only(bottom: 4.0),
                                      child: Icon(FluentIcons.car, size: 14),
                                    ),
                                  ),
                                );
                              },
                              // anchorPos: AnchorPos.align(AnchorAlign.center),
                            )
                          ],
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
