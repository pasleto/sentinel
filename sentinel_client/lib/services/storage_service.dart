// import 'package:fluent_ui/fluent_ui.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';

// TODO - It is VERY important that you have HTTP Strict Forward Secrecy enabled and the proper headers applied to your responses or you could be subject to a javascript hijack.

class StorageService {
  static const _secureStorage = FlutterSecureStorage();

  static Future<void> writeSecureData(String itemKey, String itemValue) async {
    // debugPrint('Writing new data having key $itemKey and value $itemValue');
    await _secureStorage.write(key: itemKey, value: itemValue);
  }

  static Future<String?> readSecureData(String itemKey) async {
    // debugPrint('Reading data having key $itemKey');
    var readData = await _secureStorage.read(key: itemKey);
    return readData;
  }

  static Future<void> deleteSecureData(String itemKey) async {
    // debugPrint('Deleting data having key $itemKey');
    await _secureStorage.delete(key: itemKey);
  }

  // ?
  // static Future<List<MapEntry<String, String>>> readAllSecureData() async {
  // //   debugPrint('Reading all secured data');
  //   var allData = await _secureStorage.readAll();
  //   List<MapEntry<String, String>> list = allData.entries.toList();
  //   return list;
  // }

  static Future<void> deleteAllSecureData() async {
    // debugPrint('Deleting all secured data');
    await _secureStorage.deleteAll();
  }

  static Future<bool> containsKeyInSecureData(String itemKey) async {
    // debugPrint("Checking data for the key $itemKey");
    var containsKey = await _secureStorage.containsKey(key: itemKey);
    return containsKey;
  }
}
