import React, { useState, useEffect, Fragment, useRef, useCallback } from 'react';

function ConfigPage(props) {
  const sslRootCertificateRef = useRef(null);
  const setSslRootCertificateRef = useCallback(node => {
    sslRootCertificateRef.current = node;
    sslRootCertificateRecalculateFontSize();
  }, []);
  const sslClientCertificateRef = useRef(null);
  const setSslClientCertificateRef = useCallback(node => {
    sslClientCertificateRef.current = node;
    sslClientCertificateRecalculateFontSize();
  }, []);
  const sslClientPrivateKeyRef = useRef(null);
  const setSslClientPrivateKeyRef = useCallback(node => {
    sslClientPrivateKeyRef.current = node;
    sslClientPrivateKeyRecalculateFontSize();
  }, []);
  const [changePassword, setChangePassword] = useState(false);
  const [devicePassword, setDevicePassword] = useState('');
  const [devicePasswordCheckEvent, setDevicePasswordCheckEvent] = useState(null);
  const [changeMqttPassword, setChangeMqttPassword] = useState(false);
  const [mqttPassword, setMqttPassword] = useState('');
  const [mqttPasswordCheckEvent, setMqttPasswordCheckEvent] = useState(null);

  const handleIpStaticChange = (value) => {
    props.setConfig((prevState) => ({...prevState, ip_static: value}));
  }

  const handleDevicePassword = (value) => {
    setDevicePassword(value);
    if (devicePasswordCheckEvent !== null) {
      handleDevicePasswordCheck(devicePasswordCheckEvent, value);
    }
  }

  const handleDevicePasswordCheck = (e, compare) => {
    setDevicePasswordCheckEvent(e);
    if (e.target.value !== compare) {
      e.target.setCustomValidity('Password fields must match!'); // ('⠀')
    } else {
      e.target.setCustomValidity('');
    }
  }

  const handleMqttPassword = (value) => {
    setMqttPassword(value);
    if (mqttPasswordCheckEvent !== null) {
      handleMqttPasswordCheck(mqttPasswordCheckEvent, value);
    }
  }

  const handleMqttPasswordCheck = (e, compare) => {
    setMqttPasswordCheckEvent(e);
    if (e.target.value !== compare) {
      e.target.setCustomValidity('Password fields must match!'); // ('⠀')
    } else {
      e.target.setCustomValidity('');
    }
  }

  const handleIpConfigurationMode = (value) => {
    switch (value) {
      case 'dhcp':
        handleIpStaticChange(false);
        break;
      case 'static':
        handleIpStaticChange(true);
        break;
      default:
        handleIpStaticChange(false);
        break;
    }
  }

  const sslRootCertificateRecalculateFontSize = () => {
    if (sslRootCertificateRef.current !== null) {
      sslRootCertificateRef.current.style.fontSize = sslRootCertificateRef.current.scrollWidth / props.textAreaFontRatio + 'px';
    }
  }

  const sslClientCertificateRecalculateFontSize = () => {
    if (sslClientCertificateRef.current !== null) {
      sslClientCertificateRef.current.style.fontSize = sslClientCertificateRef.current.scrollWidth / props.textAreaFontRatio + 'px';
    }
  }

  const sslClientPrivateKeyRecalculateFontSize = () => {
    if (sslClientPrivateKeyRef.current !== null) {
      sslClientPrivateKeyRef.current.style.fontSize = sslClientPrivateKeyRef.current.scrollWidth / props.textAreaFontRatio + 'px';
    }
  }

  useEffect(() => {
    window.addEventListener('resize', () => {
      sslRootCertificateRecalculateFontSize();
      sslClientCertificateRecalculateFontSize();
      sslClientPrivateKeyRecalculateFontSize();
    });
  }, []);

  const submitForm = async (e) => { // TODO
    e.preventDefault();
    props.setIsLoading(true);
    props.setLoadingHeader('Saving configuration');
    props.setLoadingMessage('');
    let payload = {
      // standalone_mode: props.config.standalone_mode, // TODO
      keep_webserver: props.config.keep_webserver,
      hostname: props.config.hostname,
      device_password_change: changePassword,
      device_password: devicePassword,
      static_ip: props.config.ip_static,
      ip: props.config.ip_static_ip,
      mask: props.config.ip_static_mask,
      gateway: props.config.ip_static_gw,
      dns: props.config.ip_static_dns,
      mqtt_server: props.config.mqtt_server,
      mqtt_port: props.config.mqtt_port,
      mqtt_credentials: props.config.mqtt_credentials,
      mqtt_username: props.config.mqtt_user,
      mqtt_password_change: changeMqttPassword,
      mqtt_password: mqttPassword,
      ssl_use: props.config.ssl_use,
      ssl_root_ca: props.config.ssl_root_ca,
      ssl_validate_client: props.config.ssl_validate_client,
      ssl_client_certificate: props.config.ssl_client_certificate,
      ssl_client_private_key: props.config.ssl_client_private_key,
      ntp: props.config.ntp,
      button_1: props.config.button_1, // !
      button_1_timeout: props.config.button_1_timeout,
      button_2: props.config.button_2, // !
      button_2_timeout: props.config.button_2_timeout,
      door_sensor: props.config.door_sensor,
      door_alarm: props.config.door_alarm, // TODO
      door_alarm_restore: props.config.door_alarm_restore, // TODO
      door_alarm_timeout: props.config.door_alarm_timeout, // TODO
      external_alarm: props.config.external_alarm, // TODO
      reader_1: props.config.reader_1, // !
      reader_1_keypad: props.config.reader_1_keypad, // !
      reader_1_buzzer: props.config.reader_1_buzzer_timeout,
      reader_1_timeout: props.config.reader_1_timeout,
      reader_1_reverse: props.config.reader_1_reverse,
      reader_1_silent: props.config.reader_1_silent,
      reader_1_tamper_sensor: props.config.reader_1_tamper_sensor,
      reader_1_tamper_invert: props.config.reader_1_tamper_invert,
      reader_1_tamper_restore: props.config.reader_1_tamper_restore,
      reader_2: props.config.reader_2, // !
      reader_2_keypad: props.config.reader_2_keypad, // !
      reader_2_buzzer: props.config.reader_2_buzzer_timeout,
      reader_2_timeout: props.config.reader_2_timeout,
      reader_2_reverse: props.config.reader_2_reverse,
      reader_2_silent: props.config.reader_2_silent,
      reader_2_tamper_sensor: props.config.reader_2_tamper_sensor,
      reader_2_tamper_invert: props.config.reader_2_tamper_invert,
      reader_2_tamper_restore: props.config.reader_2_tamper_restore,
      enclosure_tamper_sensor: props.config.enclosure_tamper_sensor,
      enclosure_tamper_invert: props.config.enclosure_tamper_invert,
      enclosure_tamper_restore: props.config.enclosure_tamper_restore,
      fire_sensor: props.config.fire_sensor,
      fire_sensor_invert: props.config.fire_sensor_invert,
      fire_sensor_restore: props.config.fire_sensor_restore,
    }

    console.log(payload); // !

    props.sendJsonMessage({
      topic: 'CONFIG-SET-REQ',
      data: payload
    });
  }

  const content = () => {
    return (
      <Fragment>
        <table className='info-table'>
          <thead>
            <tr>
              <td width='50%'></td>
              <td width='50%'></td>
            </tr>
          </thead>
          <tbody>
            <tr>
              <td align='left' colSpan='1'>Module type:</td>
              <td align='right' colSpan='1'><span id='type'>{props.config.module_type}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Firmware version:</td>
              <td align='right' colSpan='1'><span id='firmware'>{props.config.firmware}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Hardware version:</td>
              <td align='right' colSpan='1'><span id='hardware'>{props.config.hardware}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Unit ID:</td>
              <td align='right' colSpan='1'><span id='unit'>{props.config.unit_id}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>MAC Address:</td>
              <td align='right' colSpan='1'><span id='mac'>{props.config.mac_address}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>IP Address:</td>
              <td align='right' colSpan='1'><span id='mac'>{props.config.ip_address}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Storage Usage:</td>
              <td align='right' colSpan='1'><span id='storage'>{props.config.storage_usage} % <meter align='left' className='storage-meter' min={0} low={69} high={80} max={100} value={props.config.storage_usage} ></meter></span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Current Mode:</td>
              <td align='right' colSpan='1'><span id='mode'>{props.currentlyStandaloneMode ? 'Standalone' : 'Controlled'}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Configuration Mode:</td>
              <td align='right' colSpan='1'><span id='config'>{props.config.config_mode ? 'YES' : 'NO'}</span></td>
            </tr>
            <tr>
              <td align='center' colSpan='2'>&nbsp;</td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Status Page:</td>
              <td align='right' colSpan='1'><input type='button' className='link-button' value='➔' onClick={() => props.setComponentRender('status')} /></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Database Page:</td>
              <td align='right' colSpan='1'><input type='button' className='link-button' value='➔' onClick={() => props.setComponentRender('database')} /></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Logs Page:</td>
              <td align='right' colSpan='1'><input type='button' className='link-button' value='➔' onClick={() => props.setComponentRender('logs')} /></td>
            </tr>
            {/* <tr>
              <td align='center' colSpan='2'>&nbsp;</td>
            </tr>
            <tr>
              <td align='center' colSpan='2'>XXX</td>
            </tr> */}
          </tbody>
        </table>
        <form onSubmit={submitForm} >
          <table>
            <thead>
              <tr>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
                <td width='5%'></td>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td align='center' colSpan='20'>&nbsp;</td>
              </tr>
              <tr>
                <td align='left' colSpan='7'>Controlled</td>
                <td align='center' colSpan='6'>
                  <label className='switch-wide'>
                    <input type='checkbox' disabled={/*!currentlyStandaloneMode &&*/ !props.config.config_mode} checked={props.config.standalone_mode} onChange={(e) => props.setConfig((prevState) => ({...prevState, standalone_mode: e.target.checked}))} />
                    <span className='slider-wide'></span>
                  </label>
                </td>
                <td align='right' colSpan='7'>Standalone</td>
              </tr>
              {/*
              <tr>
                <td align='center' colSpan='20' className='device-mode-warning'>&nbsp;{props.config.currentlyStandaloneMode !== props.config.standalone_mode && props.config.was_configured ? 'Data on device will be overwritten!' : ''}&nbsp;</td>
              </tr>
              */}
              <tr>
                <td align='left' colSpan='9'>Hostname</td>
                <td align='right' colSpan='11'><input required={true} size='1' value={props.config.hostname} type='text' pattern='^[a-zA-Z\d]{0,}[a-zA-Z\d-]{0,}[a-zA-Z\d]{1,}$' maxLength='62' autoCapitalize='off' onChange={(e) => props.setConfig((prevState) => ({...prevState, hostname: e.target.value}))} /></td>
              </tr>
              <tr>
                <td align='left' colSpan='13'>Web Outside Config Mode</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.keep_webserver} onChange={(e) => props.setConfig((prevState) => ({...prevState, keep_webserver: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {/*
              <tr>
                <td align='left' colSpan='1'></td>
                <td align='left' colSpan='19'>
                  <table>
                    <tbody>
                    <tr>
                      <td width='15%' align='left'>ON</td>
                      <td align='left'>Website served all the time</td>
                    </tr>
                    <tr>
                      <td width='15%' align='left'>OFF</td>
                      <td align='left'>Website served only in configuration mode</td>
                    </tr>
                    </tbody>
                  </table>
                </td>
              </tr>
              */}
              <tr>
                <td align='left' colSpan='13'>Change Device Password</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={changePassword} onChange={(e) => setChangePassword(e.target.checked)} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                changePassword ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Password</td>
                  <td align='right' colSpan='11'><input required={changePassword} size='1' value={devicePassword} type='password' autoCapitalize='off' minLength={8} onChange={(e) => handleDevicePassword(e.target.value)} /></td>
                </tr>
                : null
              }
              {
                changePassword ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Password</td>
                  <td align='right' colSpan='11'><input required={changePassword} size='1' type='password' placeholder='Retype password' autoCapitalize='off' minLength={8} onChange={(e) => handleDevicePasswordCheck(e, devicePassword)} /></td>
                </tr>
                : null
              }
              <tr>
                <td align='left' colSpan='13'>IP Configuration</td>
                <td align='right' colSpan='7'>
                  <select value={props.config.ip_static ? 'static' : 'dhcp'} onChange={(e) => handleIpConfigurationMode(e.target.value)}>
                    <option value='dhcp'>DHCP</option>
                    <option value='static'>STATIC</option>
                  </select>
                </td>
              </tr>
              {
                props.config.ip_static ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>IP Address</td>
                  <td align='right' colSpan='11'><input required={props.config.ip_static} size='1' value={props.config.ip_static_ip} type='text' pattern='^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$' placeholder='192.168.1.100' autoCapitalize='off'  onChange={(e) => props.setConfig((prevState) => ({...prevState, ip_static_ip: e.target.value}))} /></td>
                </tr>
                : null
              }
              {
                props.config.ip_static ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Subnet Mask</td>
                  <td align='right' colSpan='11'><input required={props.config.ip_static} size='1' value={props.config.ip_static_mask} type='text' pattern='^((128|192|224|240|248|252|254)\.0\.0\.0)|(255\.(((0|128|192|224|240|248|252|254)\.0\.0)|(255\.(((0|128|192|224|240|248|252|254)\.0)|255\.(0|128|192|224|240|248|252|254)))))$' placeholder='255.255.255.0' autoCapitalize='off' onChange={(e) => props.setConfig((prevState) => ({...prevState, ip_static_mask: e.target.value}))} /></td>
                </tr>
                : null
              }
              {
                props.config.ip_static ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Gateway</td>
                  <td align='right' colSpan='11'><input required={props.config.ip_static} size='1' value={props.config.ip_static_gw} type='text' pattern='^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$' placeholder='192.168.1.1' autoCapitalize='off' onChange={(e) => props.setConfig((prevState) => ({...prevState, ip_static_gw: e.target.value}))} /></td>
                </tr>
                : null
              }
              {
                props.config.ip_static ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>DNS Server</td>
                  <td align='right' colSpan='11'><input required={props.config.ip_static} size='1' value={props.config.ip_static_dns} type='text' pattern='^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$' placeholder='192.168.1.1' autoCapitalize='off' onChange={(e) => props.setConfig((prevState) => ({...prevState, ip_static_dns: e.target.value}))} /></td>
                </tr>
                : null
              }
              <tr>
                <td align='left' colSpan='9'>NTP Server</td>
                <td align='center' colSpan='11'><input required={true} size='1' value={props.config.ntp} type='text' pattern='^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$' placeholder='pool.ntp.org' autoCapitalize='off' onChange={(e) => props.setConfig((prevState) => ({...prevState, ntp: e.target.value}))} /></td>
              </tr>
              <tr>
                <td align='left' colSpan='9'>MQTT Server</td>
                <td align='center' colSpan='11'><input required={true} size='1' value={props.config.mqtt_server} type='text' pattern='^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$' autoCapitalize='off' placeholder='mqtt.example.com' onChange={(e) => props.setConfig((prevState) => ({...prevState, mqtt_server: e.target.value}))} /></td>
              </tr>
              <tr>
                <td align='left' colSpan='9'>MQTT Port</td>
                <td align='center' colSpan='11'><input required={true} size='1' value={props.config.mqtt_port} type='text' pattern='^[0-9]{2,5}$' placeholder='1883' autoCapitalize='off' onChange={(e) => props.setConfig((prevState) => ({...prevState, mqtt_port: e.target.value}))} /></td>
              </tr>
              <tr>
                <td align='left' colSpan='13'>MQTT Credentials</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' checked={props.config.mqtt_credentials} onChange={(e) => props.setConfig((prevState) => ({...prevState, mqtt_credentials: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                props.config.mqtt_credentials ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Username</td>
                  <td align='center' colSpan='11'><input required={props.config.mqtt_credentials} size='1' value={props.config.mqtt_user} type='text' autoCapitalize='off' onChange={(e) => props.setConfig((prevState) => ({...prevState, mqtt_user: e.target.value}))} /></td>
                </tr>
                : null
              }
              {
                props.config.mqtt_credentials && !props.config.mqtt_pass_set ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Password</td>
                  <td align='center' colSpan='11'><input required={props.config.mqtt_credentials && !props.config.mqtt_pass_set} size='1' value={mqttPassword} type='password' autoCapitalize='off' onChange={(e) => handleMqttPassword(e.target.value)} /></td>
                </tr>
                : null
              }
              {
                props.config.mqtt_credentials && !props.config.mqtt_pass_set ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Password</td>
                  <td align='center' colSpan='11'><input required={props.config.mqtt_credentials && !props.config.mqtt_pass_set} size='1' type='password' placeholder='Retype password' autoCapitalize='off' onChange={(e) => handleMqttPasswordCheck(e, mqttPassword)} /></td>
                </tr>
                : null
              }
              {
                props.config.mqtt_credentials && props.config.mqtt_pass_set ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Change Password</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' checked={changeMqttPassword} onChange={(e) => setChangeMqttPassword(e.target.checked)} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.mqtt_credentials && props.config.mqtt_pass_set && changeMqttPassword ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='7'>Password</td>
                  <td align='center' colSpan='11'><input required={props.config.mqtt_credentials && props.config.mqtt_pass_set && changeMqttPassword} size='1' value={mqttPassword} type='password' autoCapitalize='off' onChange={(e) => handleMqttPassword(e.target.value)} /></td>
                </tr>
                : null
              }
              {
                props.config.mqtt_credentials && props.config.mqtt_pass_set && changeMqttPassword ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='7'>Password</td>
                  <td align='center' colSpan='11'><input required={props.config.mqtt_credentials && props.config.mqtt_pass_set && changeMqttPassword} size='1' type='password' placeholder='Retype password' autoCapitalize='off' onChange={(e) => handleMqttPasswordCheck(e, mqttPassword)} /></td>
                </tr>
                : null
              }
              <tr>
                <td align='left' colSpan='13'>TLS Communication</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' checked={props.config.ssl_use} onChange={(e) => props.setConfig((prevState) => ({...prevState, ssl_use: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                props.config.ssl_use ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='19' className='label'>Root CA Certificate</td>
                </tr>
                : null
              }
              {
                props.config.ssl_use ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='center' colSpan='19'><textarea ref={setSslRootCertificateRef} required={props.config.ssl_use} value={props.config.ssl_root_ca} autoCapitalize='off' placeholder='Paste here content of certificate file/s' onChange={(e) => props.setConfig((prevState) => ({...prevState, ssl_root_ca: e.target.value}))} /></td>
                </tr>
                : null
              }
              {
                props.config.ssl_use ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Validate Client</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' checked={props.config.ssl_validate_client} onChange={(e) => props.setConfig((prevState) => ({...prevState, ssl_validate_client: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.ssl_use && props.config.ssl_validate_client ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='18' className='label'>Client Certificate</td>
                </tr>
                : null
              }
              {
                props.config.ssl_use && props.config.ssl_validate_client ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='center' colSpan='18'><textarea ref={setSslClientCertificateRef} required={props.config.ssl_use && props.config.ssl_validate_client} value={props.config.ssl_client_certificate} autoCapitalize='off' placeholder='Paste here content of client certificate file' onChange={(e) => props.setConfig((prevState) => ({...prevState, ssl_client_certificate: e.target.value}))} /></td>
                </tr>
                : null
              }
              {
                props.config.ssl_use && props.config.ssl_validate_client ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='18' className='label'>Client Private Key</td>
                </tr>
                : null
              }
              {
                props.config.ssl_use && props.config.ssl_validate_client ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='center' colSpan='18'><textarea ref={setSslClientPrivateKeyRef} required={props.config.ssl_use && props.config.ssl_validate_client} value={props.config.ssl_client_private_key} autoCapitalize='off' placeholder='Paste here content of client certificate private key file' onChange={(e) => props.setConfig((prevState) => ({...prevState, ssl_client_private_key: e.target.value}))} /></td>
                </tr>
                : null
              }

              <tr>
                <td align='left' colSpan='13'>Door Sensor</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.door_sensor} onChange={(e) => props.setConfig((prevState) => ({...prevState, door_sensor: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                props.config.door_sensor  ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Door Sensor Alarm</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.door_alarm} onChange={(e) => props.setConfig((prevState) => ({...prevState, door_alarm: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.door_sensor && props.config.door_alarm ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='11'>Alarm Timeout</td>
                  <td align='right' colSpan='7'>
                    <select /*disabled={!currentlyStandaloneMode && !configMode}*/ value={props.config.door_alarm_timeout} onChange={(e) => props.setConfig((prevState) => ({...prevState, door_alarm_timeout: e.target.value}))}>
                      <option value='1000'>1000 ms</option>
                      <option value='2000'>2000 ms</option>
                      <option value='3000'>3000 ms</option>
                      <option value='4000'>4000 ms</option>
                      <option value='5000'>5000 ms</option>
                      <option value='6000'>6000 ms</option>
                      <option value='7000'>7000 ms</option>
                      <option value='8000'>8000 ms</option>
                      <option value='9000'>9000 ms</option>
                      <option value='10000'>10000 ms</option>
                      <option value='11000'>11000 ms</option>
                      <option value='12000'>12000 ms</option>
                      <option value='13000'>13000 ms</option>
                      <option value='14000'>14000 ms</option>
                      <option value='15000'>15000 ms</option>
                      <option value='16000'>16000 ms</option>
                      <option value='17000'>17000 ms</option>
                      <option value='18000'>18000 ms</option>
                      <option value='19000'>19000 ms</option>
                      <option value='20000'>20000 ms</option>
                      <option value='21000'>21000 ms</option>
                      <option value='22000'>22000 ms</option>
                      <option value='23000'>23000 ms</option>
                      <option value='24000'>24000 ms</option>
                      <option value='25000'>25000 ms</option>
                      <option value='26000'>26000 ms</option>
                      <option value='27000'>27000 ms</option>
                      <option value='28000'>28000 ms</option>
                      <option value='29000'>29000 ms</option>
                      <option value='30000'>30000 ms</option>
                      <option value='31000'>31000 ms</option>
                      <option value='32000'>32000 ms</option>
                      <option value='33000'>33000 ms</option>
                      <option value='34000'>34000 ms</option>
                      <option value='35000'>35000 ms</option>
                      <option value='36000'>36000 ms</option>
                      <option value='37000'>37000 ms</option>
                      <option value='38000'>38000 ms</option>
                      <option value='39000'>39000 ms</option>
                      <option value='40000'>40000 ms</option>
                      <option value='41000'>41000 ms</option>
                      <option value='42000'>42000 ms</option>
                      <option value='43000'>43000 ms</option>
                      <option value='44000'>44000 ms</option>
                      <option value='45000'>45000 ms</option>
                      <option value='46000'>46000 ms</option>
                      <option value='47000'>47000 ms</option>
                      <option value='48000'>48000 ms</option>
                      <option value='49000'>49000 ms</option>
                      <option value='50000'>50000 ms</option>
                      <option value='51000'>51000 ms</option>
                      <option value='52000'>52000 ms</option>
                      <option value='53000'>53000 ms</option>
                      <option value='54000'>54000 ms</option>
                      <option value='55000'>55000 ms</option>
                      <option value='56000'>56000 ms</option>
                      <option value='57000'>57000 ms</option>
                      <option value='58000'>58000 ms</option>
                      <option value='59000'>59000 ms</option>
                      <option value='60000'>60000 ms</option>
                    </select>
                  </td>
                </tr>
                : null
              }
              {
                props.config.door_sensor && props.config.door_alarm ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='11'>Alarm Auto Restore</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.door_alarm_restore} onChange={(e) => props.setConfig((prevState) => ({...prevState, door_alarm_restore: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }

              
              <tr>
                <td align='left' colSpan='13'>Button 1</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.button_1} onChange={(e) => props.setConfig((prevState) => ({...prevState, button_1: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                props.config.button_1 ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Lock Timer</td>
                  <td align='right' colSpan='7'>
                    <select /*disabled={!currentlyStandaloneMode && !configMode}*/ value={props.config.button_1_timeout} onChange={(e) => props.setConfig((prevState) => ({...prevState, button_1_timeout: e.target.value}))}>
                      <option value='1000'>1000 ms</option>
                      <option value='2000'>2000 ms</option>
                      <option value='3000'>3000 ms</option>
                      <option value='4000'>4000 ms</option>
                      <option value='5000'>5000 ms</option>
                      <option value='6000'>6000 ms</option>
                      <option value='7000'>7000 ms</option>
                      <option value='8000'>8000 ms</option>
                      <option value='9000'>9000 ms</option>
                      <option value='10000'>10000 ms</option>
                      <option value='11000'>11000 ms</option>
                      <option value='12000'>12000 ms</option>
                      <option value='13000'>13000 ms</option>
                      <option value='14000'>14000 ms</option>
                      <option value='15000'>15000 ms</option>
                      <option value='16000'>16000 ms</option>
                      <option value='17000'>17000 ms</option>
                      <option value='18000'>18000 ms</option>
                      <option value='19000'>19000 ms</option>
                      <option value='20000'>20000 ms</option>
                      <option value='21000'>21000 ms</option>
                      <option value='22000'>22000 ms</option>
                      <option value='23000'>23000 ms</option>
                      <option value='24000'>24000 ms</option>
                      <option value='25000'>25000 ms</option>
                      <option value='26000'>26000 ms</option>
                      <option value='27000'>27000 ms</option>
                      <option value='28000'>28000 ms</option>
                      <option value='29000'>29000 ms</option>
                      <option value='30000'>30000 ms</option>
                    </select>
                  </td>
                </tr>
                : null
              }
              <tr>
                <td align='left' colSpan='13'>Button 2</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.button_2} onChange={(e) => props.setConfig((prevState) => ({...prevState, button_2: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                props.config.button_2 ?
                <tr>
                <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Lock Timer</td>
                  <td align='right' colSpan='7'>
                    <select /*disabled={!currentlyStandaloneMode && !configMode}*/ value={props.config.button_2_timeout} onChange={(e) => props.setConfig((prevState) => ({...prevState, button_2_timeout: e.target.value}))}>
                      <option value='1000'>1000 ms</option>
                      <option value='2000'>2000 ms</option>
                      <option value='3000'>3000 ms</option>
                      <option value='4000'>4000 ms</option>
                      <option value='5000'>5000 ms</option>
                      <option value='6000'>6000 ms</option>
                      <option value='7000'>7000 ms</option>
                      <option value='8000'>8000 ms</option>
                      <option value='9000'>9000 ms</option>
                      <option value='10000'>10000 ms</option>
                      <option value='11000'>11000 ms</option>
                      <option value='12000'>12000 ms</option>
                      <option value='13000'>13000 ms</option>
                      <option value='14000'>14000 ms</option>
                      <option value='15000'>15000 ms</option>
                      <option value='16000'>16000 ms</option>
                      <option value='17000'>17000 ms</option>
                      <option value='18000'>18000 ms</option>
                      <option value='19000'>19000 ms</option>
                      <option value='20000'>20000 ms</option>
                      <option value='21000'>21000 ms</option>
                      <option value='22000'>22000 ms</option>
                      <option value='23000'>23000 ms</option>
                      <option value='24000'>24000 ms</option>
                      <option value='25000'>25000 ms</option>
                      <option value='26000'>26000 ms</option>
                      <option value='27000'>27000 ms</option>
                      <option value='28000'>28000 ms</option>
                      <option value='29000'>29000 ms</option>
                      <option value='30000'>30000 ms</option>
                    </select>
                  </td>
                </tr>
                : null
              }


              <tr>
                <td align='left' colSpan='13'>Reader 1</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_1} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_1: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                props.config.reader_1 ?
                <tr>
                <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Keypad</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_1_keypad} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_1_keypad: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_1 ?
                <tr>
                <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Buzzer Timer</td>
                  <td align='right' colSpan='7'>
                    <select /*disabled={!currentlyStandaloneMode && !configMode}*/ value={props.config.reader_1_buzzer_timeout} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_1_buzzer_timeout: e.target.value}))}>
                      <option value='100'>100 ms</option>
                      <option value='150'>150 ms</option>
                      <option value='200'>200 ms</option>
                      <option value='250'>250 ms</option>
                      <option value='300'>300 ms</option>
                      <option value='350'>350 ms</option>
                      <option value='400'>400 ms</option>
                      <option value='450'>450 ms</option>
                      <option value='500'>500 ms</option>
                    </select>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_1 ?
                <tr>
                <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Lock Timer</td>
                  <td align='right' colSpan='7'>
                    <select /*disabled={!currentlyStandaloneMode && !configMode}*/ value={props.config.reader_1_timeout} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_1_timeout: e.target.value}))}>
                      <option value='1000'>1000 ms</option>
                      <option value='2000'>2000 ms</option>
                      <option value='3000'>3000 ms</option>
                      <option value='4000'>4000 ms</option>
                      <option value='5000'>5000 ms</option>
                      <option value='6000'>6000 ms</option>
                      <option value='7000'>7000 ms</option>
                      <option value='8000'>8000 ms</option>
                      <option value='9000'>9000 ms</option>
                      <option value='10000'>10000 ms</option>
                      <option value='11000'>11000 ms</option>
                      <option value='12000'>12000 ms</option>
                      <option value='13000'>13000 ms</option>
                      <option value='14000'>14000 ms</option>
                      <option value='15000'>15000 ms</option>
                      <option value='16000'>16000 ms</option>
                      <option value='17000'>17000 ms</option>
                      <option value='18000'>18000 ms</option>
                      <option value='19000'>19000 ms</option>
                      <option value='20000'>20000 ms</option>
                      <option value='21000'>21000 ms</option>
                      <option value='22000'>22000 ms</option>
                      <option value='23000'>23000 ms</option>
                      <option value='24000'>24000 ms</option>
                      <option value='25000'>25000 ms</option>
                      <option value='26000'>26000 ms</option>
                      <option value='27000'>27000 ms</option>
                      <option value='28000'>28000 ms</option>
                      <option value='29000'>29000 ms</option>
                      <option value='30000'>30000 ms</option>
                    </select>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_1 ?
                <tr>
                <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Reverse Bytes</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_1_reverse} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_1_reverse: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_1 ?
                <tr>
                <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Silent Mode</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_1_silent} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_1_silent: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_1 ?
                <tr>
                <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Tamper Sensor</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_1_tamper_sensor} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_1_tamper_sensor: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_1 && props.config.reader_1_tamper_sensor ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='11'>Invert Trigger Level</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_1_tamper_invert} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_1_tamper_invert: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_1 && props.config.reader_1_tamper_sensor ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='11'>Auto Restore Alarm</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_1_tamper_restore} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_1_tamper_restore: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }


              <tr>
                <td align='left' colSpan='13'>Reader 2</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_2} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_2: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                props.config.reader_2 ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Keypad</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_2_keypad} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_2_keypad: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_2 ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Buzzer Timer</td>
                  <td align='right' colSpan='7'>
                    <select /*disabled={!currentlyStandaloneMode && !configMode}*/ value={props.config.reader_2_buzzer_timeout} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_2_buzzer_timeout: e.target.value}))}>
                      <option value='100'>100 ms</option>
                      <option value='150'>150 ms</option>
                      <option value='200'>200 ms</option>
                      <option value='250'>250 ms</option>
                      <option value='300'>300 ms</option>
                      <option value='350'>350 ms</option>
                      <option value='400'>400 ms</option>
                      <option value='450'>450 ms</option>
                      <option value='500'>500 ms</option>
                    </select>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_2 ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Lock Timer</td>
                  <td align='right' colSpan='7'>
                    <select /*disabled={!currentlyStandaloneMode && !configMode}*/ value={props.config.reader_2_timeout} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_2_timeout: e.target.value}))}>
                      <option value='1000'>1000 ms</option>
                      <option value='2000'>2000 ms</option>
                      <option value='3000'>3000 ms</option>
                      <option value='4000'>4000 ms</option>
                      <option value='5000'>5000 ms</option>
                      <option value='6000'>6000 ms</option>
                      <option value='7000'>7000 ms</option>
                      <option value='8000'>8000 ms</option>
                      <option value='9000'>9000 ms</option>
                      <option value='10000'>10000 ms</option>
                      <option value='11000'>11000 ms</option>
                      <option value='12000'>12000 ms</option>
                      <option value='13000'>13000 ms</option>
                      <option value='14000'>14000 ms</option>
                      <option value='15000'>15000 ms</option>
                      <option value='16000'>16000 ms</option>
                      <option value='17000'>17000 ms</option>
                      <option value='18000'>18000 ms</option>
                      <option value='19000'>19000 ms</option>
                      <option value='20000'>20000 ms</option>
                      <option value='21000'>21000 ms</option>
                      <option value='22000'>22000 ms</option>
                      <option value='23000'>23000 ms</option>
                      <option value='24000'>24000 ms</option>
                      <option value='25000'>25000 ms</option>
                      <option value='26000'>26000 ms</option>
                      <option value='27000'>27000 ms</option>
                      <option value='28000'>28000 ms</option>
                      <option value='29000'>29000 ms</option>
                      <option value='30000'>30000 ms</option>
                    </select>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_2 ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Reverse Bytes</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_2_reverse} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_2_reverse: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_2 ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Silent Mode</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_2_silent} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_2_silent: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_2 ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Tamper Sensor</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_2_tamper_sensor} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_2_tamper_sensor: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_2 && props.config.reader_2_tamper_sensor ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='11'>Invert Trigger Level</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_2_tamper_invert} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_2_tamper_invert: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.reader_2 && props.config.reader_2_tamper_sensor ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='11'>Auto Restore Alarm</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.reader_2_tamper_restore} onChange={(e) => props.setConfig((prevState) => ({...prevState, reader_2_tamper_restore: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }



              <tr>
                <td align='left' colSpan='13'>Enclosure Tamper Sensor</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.enclosure_tamper_sensor} onChange={(e) => props.setConfig((prevState) => ({...prevState, enclosure_tamper_sensor: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                props.config.enclosure_tamper_sensor ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Invert Trigger Level</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.enclosure_tamper_invert} onChange={(e) => props.setConfig((prevState) => ({...prevState, enclosure_tamper_invert: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.enclosure_tamper_sensor ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Auto Restore Alarm</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.enclosure_tamper_restore} onChange={(e) => props.setConfig((prevState) => ({...prevState, enclosure_tamper_restore: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }

              <tr>
                <td align='left' colSpan='13'>Fire Sensor</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.fire_sensor} onChange={(e) => props.setConfig((prevState) => ({...prevState, fire_sensor: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                props.config.fire_sensor ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Invert Trigger Level</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.fire_sensor_invert} onChange={(e) => props.setConfig((prevState) => ({...prevState, fire_sensor_invert: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                props.config.fire_sensor ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Auto Restore Alarm</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.fire_sensor_restore} onChange={(e) => props.setConfig((prevState) => ({...prevState, fire_sensor_restore: e.target.checked}))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }

              <tr>
                <td align='left' colSpan='13'>External Alarm</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!currentlyStandaloneMode && !configMode}*/ checked={props.config.external_alarm} onChange={(e) => props.setConfig((prevState) => ({...prevState, external_alarm: e.target.checked}))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>


            </tbody>
          </table>
          <input type='submit' value='Save & Reboot' />
          <h2 className='info'>A reboot is required for the changes to take effect after modifying the configuration</h2>
        </form>
        <h2 className='info'>&nbsp;</h2>
      </Fragment>
    );
  }

  return (
    <div className='content'>
      <h1 className='header'>Configuration</h1>
      { props.isLoading ? props.loadingContent() : props.isError ? props.errorContent() : content() }
    </div>
  );
}

export default ConfigPage;
