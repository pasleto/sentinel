// import 'package:sentinel_client/models/user.model.dart';
import 'package:sentinel_client/models/cfGpsPosition.model.dart';

class CfCar {
  final String id;
  /*final*/ String licencePlateNumber;
  /*final*/ String vinNumber;
  /*final*/ String? brand;
  /*final*/ String? model;
  /*final*/ int? manufactureYear;
  /*final*/ String? fuelType;
  /*final*/ DateTime? purchaseDate;
  /*final*/ DateTime? decomissionDate;
  /*final*/ num? engineKw;
  /*final*/ num? engineCcm;
  // final User? responsiblePerson;
  /*final*/ String? responsiblePersonId;
  /*final*/ String? deviceId; // TODO
  /*final*/ bool? gpsSignal; // TODO
  /*final*/ CfGpsPosition? gpsPosition; // TODO
  /*final*/ num? tempInt; // TODO
  /*final*/ String? description;

  CfCar(
    this.id,
    this.licencePlateNumber,
    this.vinNumber,
    this.brand,
    this.model,
    this.manufactureYear,
    this.fuelType,
    this.purchaseDate,
    this.decomissionDate,
    this.engineKw,
    this.engineCcm,
    this.responsiblePersonId,
    this.deviceId,
    this.gpsSignal,
    this.gpsPosition,
    this.tempInt,
    this.description,
  );

  CfCar.fromJson(Map<String, dynamic> json)
      : id = json['_id'],
        licencePlateNumber = json['licence_plate_number'],
        vinNumber = json['vin_number'],
        brand = json['brand'],
        model = json['model'],
        manufactureYear = json['manufacture_year'],
        fuelType = json['fuel_type'],
        purchaseDate = json['purchase_date'],
        decomissionDate = json['decomission_date'],
        engineKw = json['engine_kw'],
        engineCcm = json['engine_ccm'],
        responsiblePersonId = json['responsible_person'],
        deviceId = json['device_id'],
        gpsSignal = json['gps_signal'],
        gpsPosition = json['gps'],
        tempInt = json['temp_int'],
        description = json['description'];
}
