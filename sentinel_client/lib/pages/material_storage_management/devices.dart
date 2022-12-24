import 'package:fluent_ui/fluent_ui.dart' as fluent_ui;
import 'package:data_table_2/data_table_2.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:sentinel_client/services/socket_service.dart';
import 'package:sentinel_client/widgets/page.dart';
import 'package:sentinel_client/widgets/toasts.dart';

class MsDevice {
  final String id;
  final String name;
  final String deviceId;
  final String hostname;
  final String description;
  final String macAddress;
  final String ipAddress;
  final List<String> groups;

  MsDevice(this.id, this.name, this.deviceId, this.hostname, this.description, this.macAddress, this.ipAddress, this.groups);

  MsDevice.fromJson(Map<String, dynamic> json)
      : id = json['id'],
        name = json['name'],
        deviceId = json['device_id'],
        hostname = json['hostname'],
        description = json['description'],
        macAddress = json['mac_address'],
        ipAddress = json['ip_address'],
        groups = List<String>.from(json['groups']);

  Map<String, dynamic> toJson() => {
        'id': id,
        'name': name,
        'device_id': deviceId,
        'hostname': hostname,
        'description': description,
        'mac_address': macAddress,
        'ip_address': ipAddress,
        'groups': List<dynamic>.from(groups),
      };
}

class MaterialStorageManagementDevicesPage extends StatefulWidget {
  const MaterialStorageManagementDevicesPage({super.key});

  @override
  State<MaterialStorageManagementDevicesPage> createState() => _MaterialStorageManagementDevicesPageState();
}

class _MaterialStorageManagementDevicesPageState extends State<MaterialStorageManagementDevicesPage> with PageMixin {
  bool _sortAscending = true;
  int? _sortColumnIndex;
  bool _initialized = false;
  List<MsDevice> devices = [];
  List<MsDevice> fullDevices = [];

  final TextEditingController _controllerName = TextEditingController();
  final TextEditingController _controllerDeviceId = TextEditingController();
  final TextEditingController _controllerHostname = TextEditingController();
  final TextEditingController _controllerMacAddress = TextEditingController();
  final TextEditingController _controllerGroupsCount = TextEditingController();
  final TextEditingController _controllerDescription = TextEditingController();

  DataRow _getRow(int index) {
    assert(index >= 0);
    if (index >= devices.length) throw 'index > devices.length';
    final device = devices[index];
    return DataRow2.byIndex(
      index: index,
      cells: [
        DataCell(
          fluent_ui.CommandBar(
            mainAxisAlignment: MainAxisAlignment.center,
            overflowBehavior: fluent_ui.CommandBarOverflowBehavior.noWrap,
            isCompact: true,
            primaryItems: [
              fluent_ui.CommandBarButton(
                icon: const Icon(fluent_ui.FluentIcons.edit),
                label: const Text('Edit'),
                onPressed: () {
                  print('edit - id: ${device.id}');
                },
              ),
              const fluent_ui.CommandBarSeparator(),
              fluent_ui.CommandBarButton(
                icon: const Icon(fluent_ui.FluentIcons.delete),
                label: const Text('Delete'),
                onPressed: () async => await fluent_ui.showDialog<String>(
                  context: context,
                  barrierDismissible: false,
                  builder: (context) => fluent_ui.ContentDialog(
                    title: const Text('Delete device?'),
                    content: const Text("If you delete this device, you won't be able to recover it. Do you want to delete it?"),
                    actions: [
                      fluent_ui.Button(
                        child: const Text('Delete'),
                        onPressed: () {
                          print('delete device: ${device.id}');
                          // TODO - Delete here
                          Navigator.pop(context);
                        },
                      ),
                      fluent_ui.FilledButton(
                        child: const Text('Cancel'),
                        onPressed: () => Navigator.pop(context),
                      ),
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
        DataCell(
          Text(device.name),
          onTap: () => Clipboard.setData(ClipboardData(text: device.name)).then((_) {
            AppToasts.genericNotification(context: context, messageText: 'Copied to clipboard!', closeButton: false, duration: const Duration(seconds: 1));
          }),
        ),
        DataCell(
          Text(device.deviceId),
          onTap: () => Clipboard.setData(ClipboardData(text: device.deviceId)).then((_) {
            AppToasts.genericNotification(context: context, messageText: 'Copied to clipboard!', closeButton: false, duration: const Duration(seconds: 1));
          }),
        ),
        DataCell(
          Text(device.hostname),
          onTap: () => Clipboard.setData(ClipboardData(text: device.hostname)).then((_) {
            AppToasts.genericNotification(context: context, messageText: 'Copied to clipboard!', closeButton: false, duration: const Duration(seconds: 1));
          }),
        ),
        DataCell(
          Text(device.macAddress),
          onTap: () => Clipboard.setData(ClipboardData(text: device.macAddress)).then((_) {
            AppToasts.genericNotification(context: context, messageText: 'Copied to clipboard!', closeButton: false, duration: const Duration(seconds: 1));
          }),
        ),
        DataCell(
          Text('${device.groups.length}'),
          onTap: () => Clipboard.setData(ClipboardData(text: '${device.groups.length}')).then((_) {
            AppToasts.genericNotification(context: context, messageText: 'Copied to clipboard!', closeButton: false, duration: const Duration(seconds: 1));
          }),
        ),
        DataCell(
          Text(device.description),
          onTap: () => Clipboard.setData(ClipboardData(text: device.description)).then((_) {
            AppToasts.genericNotification(context: context, messageText: 'Copied to clipboard!', closeButton: false, duration: const Duration(seconds: 1));
          }),
        ),
      ],
    );
  }

  void _sort<T>(
    Comparable<T> Function(MsDevice d) getField,
    int columnIndex,
    bool ascending,
  ) {
    devices.sort((a, b) {
      final aValue = getField(a);
      final bValue = getField(b);
      return ascending ? Comparable.compare(aValue, bValue) : Comparable.compare(bValue, aValue);
    });
    setState(() {
      _sortColumnIndex = columnIndex;
      _sortAscending = ascending;
    });
  }

  late List<DataColumn2> tableColumns = [
    const DataColumn2(
      label: fluent_ui.Center(child: Text('')),
      size: ColumnSize.S,
      fixedWidth: 85,
    ),
    DataColumn2(
      label: const Text('Name', style: TextStyle(fontWeight: FontWeight.w600)),
      size: ColumnSize.L,
      tooltip: 'Device name',
      onSort: (columnIndex, ascending) => _sort<String>((d) => d.name, columnIndex, ascending),
    ),
    DataColumn2(
      label: const Text('Unit ID', style: TextStyle(fontWeight: FontWeight.w600)),
      fixedWidth: 165,
      size: ColumnSize.M,
      tooltip: 'Device identificator',
      onSort: (columnIndex, ascending) => _sort<String>((d) => d.deviceId, columnIndex, ascending),
    ),
    DataColumn2(
      label: const Text('Hostname', style: TextStyle(fontWeight: FontWeight.w600)),
      size: ColumnSize.M,
      tooltip: 'Device hostname',
      onSort: (columnIndex, ascending) => _sort<String>((d) => d.hostname, columnIndex, ascending),
    ),
    DataColumn2(
      label: const Text('Mac Address', style: TextStyle(fontWeight: FontWeight.w600)),
      fixedWidth: 185,
      size: ColumnSize.M,
      tooltip: 'Device MAC address',
      onSort: (columnIndex, ascending) => _sort<String>((d) => d.macAddress, columnIndex, ascending),
    ),
    DataColumn2(
      label: const Text('Groups', style: TextStyle(fontWeight: FontWeight.w600)),
      fixedWidth: 100,
      size: ColumnSize.S,
      tooltip: 'Number of groups',
      onSort: (columnIndex, ascending) => _sort<num>((d) => d.groups.length, columnIndex, ascending),
    ),
    DataColumn2(
      label: const Text('Description', style: TextStyle(fontWeight: FontWeight.w600)),
      size: ColumnSize.L,
      tooltip: 'Description',
      onSort: (columnIndex, ascending) => _sort<String>((d) => d.description, columnIndex, ascending),
    ),
  ];

  late List<DataRow> searchRow = [
    DataRow2(
      specificRowHeight: 42,
      cells: [
        DataCell(
          placeholder: true,
          fluent_ui.CommandBar(
            mainAxisAlignment: MainAxisAlignment.center,
            overflowBehavior: fluent_ui.CommandBarOverflowBehavior.noWrap,
            isCompact: true,
            primaryItems: [
              const fluent_ui.CommandBarButton(
                icon: Icon(fluent_ui.FluentIcons.search),
                label: Text('Search filters'),
                onPressed: null,
              ),
              const fluent_ui.CommandBarSeparator(),
              fluent_ui.CommandBarButton(
                // icon: const Icon(fluent_ui.FluentIcons.clear_filter),
                icon: const Icon(fluent_ui.FluentIcons.remove_filter),
                label: const Text('Clear filters'),
                onPressed: () {
                  _controllerName.clear();
                  _controllerDeviceId.clear();
                  _controllerHostname.clear();
                  _controllerMacAddress.clear();
                  _controllerGroupsCount.clear();
                  _controllerDescription.clear();
                },
              ),
            ],
          ),
        ),
        DataCell(
          placeholder: true,
          fluent_ui.TextBox(
            maxLines: 1,
            minLines: 1,
            textAlignVertical: TextAlignVertical.center,
            controller: _controllerName,
            suffix: GestureDetector(
              onTap: () => _controllerName.clear(),
              child: const Padding(
                padding: EdgeInsets.only(right: 6.0),
                child: Icon(Icons.close_rounded),
              ),
            ),
          ),
        ),
        DataCell(
          placeholder: true,
          fluent_ui.TextBox(
            maxLines: 1,
            minLines: 1,
            textAlignVertical: TextAlignVertical.center,
            controller: _controllerDeviceId,
            suffix: GestureDetector(
              onTap: () => _controllerDeviceId.clear(),
              child: const Padding(
                padding: EdgeInsets.only(right: 6.0),
                child: Icon(Icons.close_rounded),
              ),
            ),
          ),
        ),
        DataCell(
          placeholder: true,
          fluent_ui.TextBox(
            maxLines: 1,
            minLines: 1,
            textAlignVertical: TextAlignVertical.center,
            controller: _controllerHostname,
            suffix: GestureDetector(
              onTap: () => _controllerHostname.clear(),
              child: const Padding(
                padding: EdgeInsets.only(right: 6.0),
                child: Icon(Icons.close_rounded),
              ),
            ),
          ),
        ),
        DataCell(
          placeholder: true,
          fluent_ui.TextBox(
            maxLines: 1,
            minLines: 1,
            textAlignVertical: TextAlignVertical.center,
            controller: _controllerMacAddress,
            suffix: GestureDetector(
              onTap: () => _controllerMacAddress.clear(),
              child: const Padding(
                padding: EdgeInsets.only(right: 6.0),
                child: Icon(Icons.close_rounded),
              ),
            ),
          ),
        ),
        DataCell(
          placeholder: true,
          fluent_ui.TextBox(
            maxLines: 1,
            minLines: 1,
            keyboardType: TextInputType.number,
            inputFormatters: [FilteringTextInputFormatter.digitsOnly],
            textAlignVertical: TextAlignVertical.center,
            controller: _controllerGroupsCount,
            suffix: GestureDetector(
              onTap: () => _controllerGroupsCount.clear(),
              child: const Padding(
                padding: EdgeInsets.only(right: 6.0),
                child: Icon(Icons.close_rounded),
              ),
            ),
          ),
        ),
        DataCell(
          placeholder: true,
          fluent_ui.TextBox(
            maxLines: 1,
            minLines: 1,
            textAlignVertical: TextAlignVertical.center,
            controller: _controllerDescription,
            suffix: GestureDetector(
              onTap: () => _controllerDescription.clear(),
              child: const Padding(
                padding: EdgeInsets.only(right: 6.0),
                child: Icon(Icons.close_rounded),
              ),
            ),
          ),
        ),
      ],
    )
  ];

  void _filterArray() {
    setState(() {
      devices = fullDevices
          .where(
            (device) =>
                (_controllerName.text.isNotEmpty ? device.name.toLowerCase().contains(_controllerName.text.toLowerCase()) : true) &&
                (_controllerDeviceId.text.isNotEmpty ? device.deviceId.toLowerCase().contains(_controllerDeviceId.text.toLowerCase()) : true) &&
                (_controllerHostname.text.isNotEmpty ? device.hostname.toLowerCase().contains(_controllerHostname.text.toLowerCase()) : true) &&
                (_controllerMacAddress.text.isNotEmpty ? device.macAddress.toLowerCase().contains(_controllerMacAddress.text.toLowerCase()) : true) &&
                (_controllerGroupsCount.text.isNotEmpty ? device.groups.length.toString().toLowerCase() == _controllerGroupsCount.text.toLowerCase() : true) &&
                (_controllerDescription.text.isNotEmpty ? device.description.toLowerCase().contains(_controllerDescription.text.toLowerCase()) : true),
          )
          .toList();
    });
  }

  @override
  void initState() {
    super.initState();
    _controllerName.addListener(_filterArray);
    _controllerDeviceId.addListener(_filterArray);
    _controllerHostname.addListener(_filterArray);
    _controllerMacAddress.addListener(_filterArray);
    _controllerGroupsCount.addListener(_filterArray);
    _controllerDescription.addListener(_filterArray);
    SocketService.socket.emitWithAck('ms-device-get-all', {}, ack: (response) {
      if (response['status'] == 'OK') {
        Iterable incoming = response['data']['devices'];
        setState(() {
          devices = List<MsDevice>.from(incoming.map((model) => MsDevice.fromJson(model)));
          fullDevices = List<MsDevice>.from(incoming.map((model) => MsDevice.fromJson(model)));
          _initialized = true;
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
        });
      }
    });
  }

  @override
  void dispose() {
    _controllerName.dispose();
    _controllerDeviceId.dispose();
    _controllerHostname.dispose();
    _controllerMacAddress.dispose();
    _controllerGroupsCount.dispose();
    _controllerDescription.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return fluent_ui.ScaffoldPage(
      header: fluent_ui.PageHeader(
        title: const Text('Material Storage Devices', style: TextStyle(fontSize: 18.0, fontWeight: FontWeight.w600)),
        commandBar: fluent_ui.CommandBarCard(
          padding: const EdgeInsets.symmetric(horizontal: 6.0, vertical: 2.0),
          child: fluent_ui.CommandBar(
            mainAxisAlignment: MainAxisAlignment.end,
            overflowBehavior: fluent_ui.CommandBarOverflowBehavior.noWrap,
            isCompact: true,
            primaryItems: [
              if (!_initialized)
                fluent_ui.CommandBarBuilderItem(
                  builder: (context, mode, w) => fluent_ui.Tooltip(
                    displayHorizontally: false,
                    useMousePosition: false,
                    style: const fluent_ui.TooltipThemeData(
                      showDuration: Duration(milliseconds: 500),
                      waitDuration: Duration(milliseconds: 500),
                      preferBelow: true,
                    ),
                    message: "Fetching data",
                    child: w,
                  ),
                  wrappedItem: const fluent_ui.CommandBarButton(
                    icon: fluent_ui.Padding(
                      padding: EdgeInsets.only(right: 12.0),
                      child: fluent_ui.SizedBox(
                        height: 16,
                        width: 16,
                        child: fluent_ui.ProgressRing(strokeWidth: 3.5),
                      ),
                    ),
                    onPressed: null,
                  ),
                ),
              fluent_ui.CommandBarBuilderItem(
                builder: (context, mode, w) => fluent_ui.Tooltip(
                  displayHorizontally: false,
                  useMousePosition: false,
                  style: const fluent_ui.TooltipThemeData(
                    showDuration: Duration(milliseconds: 500),
                    waitDuration: Duration(milliseconds: 500),
                    preferBelow: true,
                  ),
                  message: "Add device (manually)",
                  child: w,
                ),
                wrappedItem: fluent_ui.CommandBarButton(
                  icon: const Icon(fluent_ui.FluentIcons.add),
                  label: const Text('New'),
                  onPressed: () {
                    // TODO
                    print('New MS Device request - manually');
                  },
                ),
              ),
              const fluent_ui.CommandBarSeparator(),
              fluent_ui.CommandBarBuilderItem(
                builder: (context, mode, w) => fluent_ui.Tooltip(
                  displayHorizontally: false,
                  useMousePosition: false,
                  style: const fluent_ui.TooltipThemeData(
                    showDuration: Duration(milliseconds: 500),
                    waitDuration: Duration(milliseconds: 500),
                    preferBelow: true,
                  ),
                  message: "Add device (auto-staging)",
                  child: w,
                ),
                wrappedItem: fluent_ui.CommandBarButton(
                  icon: const Icon(fluent_ui.FluentIcons.box_addition_solid),
                  label: const Text('New'),
                  onPressed: () {
                    // TODO
                    print('New MS Device request - auto staging');
                  },
                ),
              ),
            ],
          ),
        ),
      ),
      content: fluent_ui.Column(
        children: [
          Expanded(
            child: Material(
              color: Colors.transparent,
              child: fluent_ui.Padding(
                padding: const EdgeInsets.only(left: 8.0, right: 8.0, bottom: 12.0),
                child: DataTable2(
                  headingRowHeight: 42,
                  columnSpacing: 12,
                  horizontalMargin: 12,
                  dividerThickness: 1,
                  bottomMargin: 10,
                  minWidth: 900,
                  sortColumnIndex: _sortColumnIndex,
                  sortAscending: _sortAscending,
                  sortArrowIcon: Icons.keyboard_arrow_up,
                  sortArrowAnimationDuration: const Duration(milliseconds: 500),
                  fixedLeftColumns: 2,
                  fixedTopRows: 2,
                  columns: tableColumns,
                  rows: [
                    ...searchRow,
                    ...List<DataRow>.generate(devices.length, (index) => _getRow(index)),
                  ],
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}
