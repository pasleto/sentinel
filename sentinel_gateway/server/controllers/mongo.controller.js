import mongoose from 'mongoose';
import logSymbols from 'log-symbols';

import timezoneService from '../models/timezone/timezone.service.js';
import settingService from '../models/setting/setting.service.js';
import departmentService from '../models/department/department.service.js';
import cardTypeService from '../models/card/type/cardType.service.js';
import cardService from '../models/card/card.service.js';
import userService from '../models/user/user.service.js';
import acInitiatorService from '../models/access_control/initiator/acInitiator.service.js';
import acEventService from '../models/access_control/event/acEvent.service.js';
import msCurrencyService from '../models/material_storage/currency/msCurrency.service.js';
import msMeasureUnitService from '../models/material_storage/measureUnit/msMeasureUnit.service.js';
import msVendorService from '../models/material_storage/vendor/msVendor.service.js';
import msProductService from '../models/material_storage/product/msProduct.service.js';
import msSupplierService from '../models/material_storage/supplier/msSupplier.service.js';
import msMaterialService from '../models/material_storage/material/msMaterial.service.js';
import msDeviceService from '../models/material_storage/device/msDevice.service.js';
import msGroupService from '../models/material_storage/group/msGroup.service.js';
import msSupplyService from '../models/material_storage/supply/msSupply.service.js';

import cfCarService from '../models/car_fleet/car/cfCar.service.js';
import cfDeviceService from '../models/car_fleet/device/cfDevice.service.js';

import cfTestService from '../models/car_fleet/test/cfTest.service.js';

async function mongoConnect() {
  try {
    // mongoose.set('strictQuery', true);
    mongoose.set('strictQuery', false);
    const result = await mongoose.connect(process.env.MONGO_URI || 'mongodb://localhost:27017/sentinel');
    console.log(logSymbols.info, `[MONGODB] Connected to ${result.connection.host}:${result.connection.port}/${result.connection.name}`);
  } catch (error) {
    console.log(logSymbols.error, `[MongoDB] Unable to connect to database`, `\n${error}`)
  }
}

async function mongoInit() { // init database structure
  await timezoneService.init();
  await settingService.init();
  await acInitiatorService.init();
  await acEventService.init();
  await departmentService.init();
  await userService.init();
  await cardTypeService.init();
  await msCurrencyService.init();
  await msMeasureUnitService.init();

  // TODO

  console.log(logSymbols.info, '[MONGODB]', 'Initialization done');

  // console.log('socketio: ');
  // console.log(global.io);

  // https://mongoosejs.com/docs/guide.html
  // https://mongoosejs.com/docs/api.html
  // https://mongoosejs.com/docs/api/document.html
  // https://mongoosejs.com/docs/documents.html
  // https://mongoosejs.com/docs/api/model.html

  // ? - Testing

  // await cfDeviceService.create({
  //   name: 'Test-1',
  //   device_id: '111111111111',
  //   mac_address: '11:11:11:11:11:11',
  //   description: 'Test 1',
  // });

  // await cfDeviceService.create({
  //   name: 'Test-2',
  //   device_id: '222222222222',
  //   mac_address: '22:22:22:22:22:22',
  //   description: 'Test 2',
  // });

  // await cfDeviceService.create({
  //   name: 'STL-CF-01',
  //   device_id: '70B8F606E08C',
  //   mac_address: '70:B8:F6:06:E0:8C',
  //   description: 'ESP32 01',
  // });

  // await cfCarService.create({
  //   licence_plate_number: '1H1 1111',
  //   vin_number: '11111111111'
  // });

  // await cfCarService.create({
  //   licence_plate_number: '2H2 2222',
  //   vin_number: '22222222222',
  //   brand: 'Škoda',
  //   model: 'Octavia',
  //   manufacture_year: 2019,
  //   fuel_type: 'Diesel',
  //   engine_kw: 110,
  //   engine_ccm: 1968,
  // });

  // ldap.allUserSync();

  // var ldap_password = await settingService.getOne({scope: 'ldap', name: 'password'});
  // ldap_password.value = 'test_password';
  // ldap_password.save();

  // var user = await UserModel.findOne({username: 'sentinel'});
  // user.password = 'password';
  // user.save();

  // var tz = await settingService.getOne({scope: 'general', name: 'timezone'});
  // tz.value = 'Etc_UTC';
  // tz.save();

  // console.log(util.getServerMacAddress());
  // console.log(await util.getServerHostname());
  // console.log(await util.getServerDomain());
  // console.log(await util.getServerDomainHostname());
  // console.log(await util.getServerUpdatesCount());

  // var test = await settingService.getOne({scope: 'proxy', name: 'username'});
  // test.value = 'sentinel_';
  // test.save();

  // var t2 = await settingService.getOne({scope: 'proxy', name: 'url_string'});
  // t2.value = '';
  // t2.save();

  // var t0 = '--bd741719500c6d359c86e94ac4509ab9a2a82dffa4d9455cb93876825f6fd0efGKwOiBQpteFMJLphWINGFw==0049304757cab16805be17458c4bbb68aae5324fcda6d63846029defef74e53a';
  // auth.materialStorageCardBarcodeAuthenticate(t0, 'TODO', function(output) {
  //   console.log(output); // ABC
  // });

  // var t1 = await utils.cryptCard('ABC');
  // console.log('Crypted: ' + t1);
  // var t2 = await utils.decryptCard(t1);
  // console.log('Decrypted: ' + t2);

  // var user = await userService.getOne({ username: 'sentinel' });
  // console.log(user);

  // await departmentService.create({
  //   'department_number': '105604',
  //   'name': 'IT_04',
  //   'description': 'IT 04',
  //   'is_root': false,
  //   'can_have_child': true,
  //   'ldap_sync': false,
  //   'parent': '6376226bdeae0265feba18e2',
  //   'is_protected': false,
  // });

  // cardService.create({
  //   value: "A4DCCC1D",
  //   type: "630d00241f04cfa4f60b6ac5",		// MIFARE Classic S50 32-bit CSN/UID
  //   has_validity: false,
  //   is_enabled: true,
  //   assigned_by: "630a510adb378feb8f989549",		// sentinel
  //   last_changed_by: "630a510adb378feb8f989549",	// sentinel
  //   user: "630a510adb378feb8f989549",		// sentinel
  // });
  // cardService.create({
  //   value: "F4A4B501",
  //   type: "630d00241f04cfa4f60b6ac5",		// MIFARE Classic S50 32-bit CSN/UID
  //   has_validity: false,
  //   is_enabled: true,
  //   assigned_by: "630a510adb378feb8f989549",		// sentinel
  //   last_changed_by: "630a510adb378feb8f989549",	// sentinel
  //   user: "630a510adb378feb8f989549"		// sentinel
  // });
  // cardService.create({
  //   value: "04C2B401",
  //   type: "630d00241f04cfa4f60b6ac5",		// MIFARE Classic S50 32-bit CSN/UID
  //   has_validity: false,
  //   is_enabled: true,
  //   assigned_by: "630a510adb378feb8f989549",		// sentinel
  //   last_changed_by: "630a510adb378feb8f989549",	// sentinel
  //   user: "6304ff6e26a4e4e03a67e06f"		// pasleto1
  // });
  // cardService.create({
  //   value: "F4EE851D",
  //   type: "630d00241f04cfa4f60b6ac1",		// Generic HF CSN/UID Block
  //   has_validity: false,
  //   is_enabled: true,
  //   assigned_by: "630a510adb378feb8f989549",		// sentinel
  //   last_changed_by: "630a510adb378feb8f989549",	// sentinel
  //   user: "632610471df848711b0a108b"		// test02
  // });

  // var group = await msGroupService.getOne({name: "Test 01"});
  // console.log(group);
  // var device = await msDeviceService.getOne({device_id: "4CCC6AAEED88"});
  // console.log(device);
  // group.devices.push(device._id);
  // await group.save();
  // device.groups.push(group._id);
  // await device.save();

};

export {
  mongoConnect,
  mongoInit,
  timezoneService,
  settingService,
  userService,
  cardTypeService,
  cardService,
  departmentService,
  acInitiatorService,
  acEventService,
  msCurrencyService,
  msMeasureUnitService,
  msVendorService,
  msProductService,
  msSupplierService,
  msMaterialService,
  msDeviceService,
  msGroupService,
  msSupplyService,
  cfCarService,
  cfDeviceService,

  cfTestService,
};
