import Aedes from 'aedes';
import utils from '../utils/util.js';
import auth from './auth.controller.js';
import { cfTestService, settingService, cfDeviceService } from './mongo.controller.js';

function handle() {
  const aedesServer = new Aedes();
  aedesServer.id = "sentinel-gateway-server";

  aedesServer.authenticate = async (client, username, password, callback)  =>  { // TODO
    // if (client.id === 'sentinel-gateway-client') {
    //   if (username === 'sentinel') {
    //     if (password && password.length > 0) {
    //       if (password.toString() === utils.getServerMachineId()) { // TODO - validate password
    //         return callback(null, true); // allow
    //       }
    //       return callback(null, false); // deny
    //     }
    //     return callback(null, false); // deny
    //   }
    //   return callback(null, false); // deny
    // } else {

      // console.log(client.id);
      // console.log(username);
      // console.log(password.toString());

      // TODO - here check whether client.id is in database

      if (username) {
        if (username === 'hash') {
          if (password && password.length > 0) { // TODO - validate  hash for client.id

            // ? each device will have separated hash, which can be used as password
            
            console.log("client-id:", client.id);
            console.log("username:", username);
            console.log("password:", password.toString());

            return callback(null, true); // allow

          }
          return callback(null, false); // deny 
        } 
        if (password && password.length > 0) {
          return await auth.authenticateMqttAccount(username, password.toString(), callback);
        }
        return callback(null, false); // deny 
      }
      return callback(null, false); // deny 

    // }
  };



  aedesServer.on('client', function (client) {
    console.log(`--- client - ${client.id}`);
  });

  aedesServer.on('clientReady', function (client) {
    console.log(`--- clientReady - ${client.id}`);
  });

  aedesServer.on('clientDisconnect', function (client) {
    console.log(`--- clientDisconnect - ${client.id}`);
  });

  aedesServer.on('clientError', function (client, error) {
    console.log(`--- clientError - ${client.id}`);
  });

  aedesServer.on('connectionError', function (client, error) {
    console.log(`--- connectionError - ${client.id} - ${error}`);
  });

  aedesServer.on('keepaliveTimeout', function (client) {
    console.log(`--- keepaliveTimeout - ${client.id}`);
  });

  aedesServer.on('ack', function (packet, client) {
    console.log(`--- ack - ${client.id}`);
  });

  aedesServer.on('ping', function (packet, client) {
    console.log(`--- ping - ${client.id}`);
  });

  aedesServer.on('subscribe', function (subscriptions, client) {
    console.log(`--- subscribe - ${client.id}`);
  });

  aedesServer.on('unsubscribe', function (unsubscriptions, client) {
    console.log(`--- unsubscribe - ${client.id}`);
  });

  aedesServer.on('connackSent', function (packet, client) {
    console.log(`--- connackSent - ${client.id}`);
  });

  aedesServer.on('closed', function () {
    console.log(`--- closed`);
  });


  aedesServer.on('publish', async function (packet, client) {
    if (client) {
      // console.log('message from client', client.id);
      // console.log(global.mqtt);
      // console.log(global.mqtt.connectedClients);
      if (packet) {
        var topic_arr = packet.topic.split('/');

        switch (topic_arr[0]) {
          case 'car-fleet':
            try {
              var cfDevice = await cfDeviceService.getOne(false, false, { device_id: topic_arr[1] }, '-createdAt -updatedAt -__v');
              // ? - from cfDevice get ref _id for cfCar
              // ? - find cfCar based on ref equals to cfDevice _id

              if (cfDevice.car != null || cfDevice.car != undefined) {

                // console.log('CfDevice:', cfDevice); // ?

                switch (topic_arr[2]) {
                  case 'status': // ?
                    // console.log(`${topic_arr[1]} | ${topic_arr[2]}: ${packet.payload.toString()}`); // ?
                    // console.log(`${client.id} | ${topic_arr[2]}: ${packet.payload.toString()}`);
                    // TODO
                    global.io.of('client-app').emit('car-fleet-status', {
                      unit_id: topic_arr[1],
                      status: packet.payload.toString()
                    });
                    break;
                  case 'realtime': // TODO - save into cfGpsPosition with ref to cfCar | broadcast socket infor with ref to cfCar
    
                    console.log(`${topic_arr[1]} | ${topic_arr[2]}: ${packet.payload.toString()}`);
                    // console.log(`${client.id} | ${topic_arr[2]}: ${packet.payload.toString()}`);
    
                    var json = JSON.parse(packet.payload.toString());
    
                    // TODO - based on topic_arr[1] find cfDevice
                    // TODO - based on cfDevice find cfCar

                    var temp = {
                      car: cfDevice.car,
                      time: json.time * 1000,
                      temp_int: json.temp.int,
                      gps_signal: json.gps_signal,
                      gps: {
                        gps_lat: json.gps.lat,
                        gps_lon: json.gps.lon,
                        gps_alt: json.gps.alt,
                        gps_speed: json.gps.speed,
                        gps_accuracy: json.gps.accuracy,
                        gps_time: json.gps.time * 1000
                      }
                    }
    
                    var with_gps = {
                      unit_id: topic_arr[1],
                      time: new Date(json.time * 1000),
                      temp_int: json.temp.int,
                      gps_signal: json.gps_signal,
                      gps_lat: json.gps.lat,
                      gps_lon: json.gps.lon,
                      gps_alt: json.gps.alt,
                      gps_speed: json.gps.speed,
                      gps_accuracy: json.gps.accuracy,
                      gps_time: new Date(json.gps.time * 1000)
                    };
    
                    var without_gps = {
                      unit_id: topic_arr[1],
                      time: new Date(json.time * 1000),
                      temp_int: json.temp.int,
                      gps_signal: json.gps_signal,
                    };
    
                    if (json.gps_signal) {
                      await cfTestService.create(with_gps);

                      global.io.of('client-app').emit('car-fleet-realtime', temp);
                    } else {
                      await cfTestService.create(without_gps);

                      global.io.of('client-app').emit('car-fleet-realtime', temp);
                    }
                    break;
                  default:
                    break;
                }
              }
            } catch (error) {
              console.log('[MQTT]', packet.topic , '- Error:' , error.message);
            }
            break;
          case 'access-control':
            try {

              // ?
          
              switch (topic_arr[2]) {
                case 'status': // ?
                  // console.log(`${topic_arr[1]} | ${topic_arr[2]}: ${packet.payload.toString()}`); // ?
                  console.log(`${client.id} | ${topic_arr[2]}: ${packet.payload.toString()}`);
                  // TODO
                  break;
                default:
                  break;
              }
            } catch (error) {
              console.log('[MQTT]', packet.topic , '- Error:' , error.message);
            }
            break;
          case 'yyy':
            // ?
            break;
          default:
            break;
        }
      }
    }
  });


  return aedesServer;
}

export default {
  handle,
};