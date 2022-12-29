import * as mqtt from "mqtt";
import Aedes from 'aedes';
import logSymbols from 'log-symbols';
import utils from '../utils/util.js';
import auth from './auth.controller.js';
import { cfTestService, settingService } from './mongo.controller.js';

function server() {
  const aedesServer = new Aedes();
  aedesServer.id = "sentinel-gateway-server";

  aedesServer.authenticate = async (client, username, password, callback)  =>  { // TODO
    if (client.id === 'sentinel-gateway-client') {
      if (username === 'sentinel') {
        if (password.length > 0) {
          if (password.toString() === utils.getServerMachineId()) { // TODO - validate password
            return callback(null, true); // allow
          }
          return callback(null, false); // deny
        }
        return callback(null, false); // deny
      }
      return callback(null, false); // deny
    } else {

      // TODO - here check whether client.id is in database

      if (username) {
        if (username === 'hash') {
          if (password.length > 0) { // TODO - validate  hash for client.id

            // ? each device will have separated hash, which can be used as password
            
            console.log("client-id:", client.id);
            console.log("username:", username);
            console.log("password:", password.toString());

            return callback(null, true); // allow

          }
          return callback(null, false); // deny 
        } 
        if (password.length > 0) {
          return await auth.authenticateMqttAccount(username, password.toString(), callback);
        }
        return callback(null, false); // deny 
      }
      return callback(null, false); // deny 
    }
  };

  // aedesServer.on('clientDisconnect', function (client) {
  //   console.log('clientDisconnect');
  //   console.log(client);
  // });

  // aedesServer.on('client', function (client) {
  //   console.log('client');
  //   console.log(client);
  // });

  // aedesServer.on('clientReady', function (client) {
  //   console.log('clientReady');
  //   console.log(client);
  // });

  // aedesServer.on('keepaliveTimeout', function (client) {
  //   console.log('keepaliveTimeout');
  //   console.log(client);
  // });

  // aedesServer.on('publish', function (packet, client) {
  //   console.log('publish');
  //   console.log(packet);
  //   console.log(client);
  // });

  // aedesServer.on('', function () {

  // });

  // aedesServer.on('publish', function (packet, client) {
  //   if (client) {
  //     console.log('message from client', client.id)
  //     if (packet && packet.payload) {
  //       console.log('publish packet:', packet.payload.toString())
  //     }
  //   }
  // });


  return aedesServer;
}


function client() {

  // var mqtt_options = {
  //   host: 'sentinel-cloud-relay.pasler.org',
  //   port: 1883,
  //   clientId: 'sentinel-gateway-client',
  // }

  var mqtt_options = {
    host: 'localhost',
    port: 1883,
    clientId: 'sentinel-gateway-client',
    username: 'sentinel',
    password: utils.getServerMachineId(),
  }

  const client = mqtt.connect(mqtt_options);

  client.on('connect', function () {
    console.log(logSymbols.info, `[Mqtt] Client connected to mqtt://${mqtt_options.host}:${mqtt_options.port}/`);

    global.mqtt = client;

    client.subscribe('#', function (err) { // subscribe to everything
      if (!err) {
        console.log(logSymbols.info, '[Mqtt] Client subscribed to all topics');
      } else {
        console.log(`[Mqtt] Client error subscribing - ${err}`);
      }
    });
  });

  client.on('error', function (err) {
    if (err) {
      console.log(`[Mqtt] Client error - ${err}`);
    }
  });
  
  client.on('close', function () {
    console.log('[Mqtt] Client disconnected');
  });

  client.on('reconnect', function () {
    console.log('[Mqtt] Client reconnecting');
  });

  client.on('message', async function (topic, payload, packet) {
    var topic_arr = topic.split('/');

    switch (topic_arr[0]) {
      case 'car-fleet':
        switch (topic_arr[2]) {
          case 'status':
            console.log(`${topic_arr[1]} | ${topic_arr[2]}: ${payload.toString()}`);

            // TODO
            global.io.of('client-app').emit('car-fleet-status', {
              unit_id: topic_arr[1],
              status: payload.toString()
            });

            break;
          case 'realtime': // TODO

            console.log(`${topic_arr[1]} | ${topic_arr[2]}: ${payload.toString()}`);

            var json = JSON.parse(payload.toString());

            // TODO - based on topic_arr[1] find cfDevice
            // TODO - based on cfDevice find cfCar

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
              global.io.of('client-app').emit('car-fleet-realtime', with_gps);
            } else {
              await cfTestService.create(without_gps);
              global.io.of('client-app').emit('car-fleet-realtime', with_gps);
            }

            break;
          default:
            break;
        }
        break;
      case 'xxx':
        // TODO
        break;
      case 'yyy':
        // TODO
        break; 
      default:
        break;
    }
    // message is Buffer
    // console.log(topic);
    // console.log(payload.toString());
    // console.log(packet);
  });


  
}

export default {
  client,
  server,
};