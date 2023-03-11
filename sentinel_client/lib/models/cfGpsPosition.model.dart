class CfGpsPosition {
  final num lat;
  final num lon;
  final num alt;
  final num speed;
  final num accuracy;
  final DateTime time;

  CfGpsPosition(
    this.lat,
    this.lon,
    this.alt,
    this.speed,
    this.accuracy,
    this.time,
  );

  CfGpsPosition.fromJson(Map<String, dynamic> json)
      : lat = json['gps_lat'],
        lon = json['gps_lon'],
        alt = json['gps_alt'],
        speed = json['gps_speed'],
        accuracy = json['gps_accuracy'],
        time = DateTime.fromMillisecondsSinceEpoch(json['gps_time']);
}
