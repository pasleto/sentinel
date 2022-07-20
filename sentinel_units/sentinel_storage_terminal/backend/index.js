require('dotenv').config();
const getmac = require('getmac')
const io = require('socket.io-client');
const socket = io.connect(process.env.SENTINEL_URI, { reconnect: true, rejectUnauthorized: false /*, secure: true*/  });
const unit_mac = getmac.default().toUpperCase();
const unit_mac_nag = getmac.default().replace(/:/g,'').toUpperCase();
const unit_name = `STL-AC-ST-${unit_mac_nag}`;

console.log(unit_mac);
console.log(unit_mac_nag);
console.log(unit_name);

// Add a connect listener
socket.on('connect', (data) => {
  console.log('Connected!');
  socket.emit('sentinel-access', `${unit_name}`, 'Hello!');
});

// socket.on('sentinel-hello', (data) => {
//   console.log('Sentinel Hello Received');
  
// });


