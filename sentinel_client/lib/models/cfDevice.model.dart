// import 'package:sentinel_client/models/cfCar.model.dart';

class CfDevice {
  final String id;
  /*final*/ String name;
  /*final*/ String deviceId;
  /*final*/ String macAddress;
  // final CfCar car;
  /*final*/ String? carId; // TODO
  /*final*/ String? description;

  CfDevice(
    this.id,
    this.name,
    this.deviceId,
    this.macAddress,
    this.carId,
    this.description,
  );

  CfDevice.fromJson(Map<String, dynamic> json)
      : id = json['_id'],
        name = json['name'],
        deviceId = json['device_id'],
        macAddress = json['mac_address'],
        carId = json['car'],
        description = json['description'];
}
