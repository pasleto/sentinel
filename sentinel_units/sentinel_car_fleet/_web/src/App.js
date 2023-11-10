import React, { useState, useEffect, Fragment, useRef, useCallback } from 'react';
import Axios from 'axios';

function App() {
  const textAreaFontRatio = 40.82355;
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
  const [isLoading, setIsLoading] = useState(true);
  const [loadingHeader, setLoadingHeader] = useState('');
  const [loadingMessage, setLoadingMessage] = useState('');
  const [isError, setIsError] = useState(false);
  const [errorMessage, setErrorMessage] = useState('');
  const [wasConfigured, setWasConfigured] = useState(false); // TODO
  const [configMode, setConfigMode] = useState(true); // TODO
  const [moduleType, setModuleType] = useState('');
  const [firmwareVersion, setFirmwareVersion] = useState('');
  // const [hardwareVersion, setHardwareVersion] = useState('');
  const [unitId, setUnitId] = useState('');
  const [macAddress, setMacAddress] = useState('');
  const [hostname, setHostname] = useState('');
  const [changePassword, setChangePassword] = useState(false);
  const [devicePassword, setDevicePassword] = useState('');
  const [devicePasswordCheckEvent, setDevicePasswordCheckEvent] = useState(null);
  const [mqttServer, setMqttServer] = useState('');
  const [mqttPort, setMqttPort] = useState('');
  const [mqttCredentials, setMqttCredentials] = useState(false);
  const [mqttUsername, setMqttUsername] = useState('');
  const [mqttPasswordSet, setMqttPasswordSet] = useState(false);
  const [changeMqttPassword, setChangeMqttPassword] = useState(false);
  const [mqttPassword, setMqttPassword] = useState('');
  const [mqttPasswordCheckEvent, setMqttPasswordCheckEvent] = useState(null);
  const [sslUse, setSslUse] = useState(false);
  const [sslRootCertificate, setSslRootCertificate] = useState('');
  const [sslValidateClient, setSslValidateClient] = useState(false);
  const [sslClientCertificate, setSslClientCertificate] = useState('');
  const [sslClientPrivateKey, setSslClientPrivateKey] = useState('');
  // const [ntpServer, setNtpServer] = useState(''); // ?
  const [gnssMode, setGnssMode] = useState(5);
  const [gprsApn, setGprsApn] = useState('');
  const [gprsCredentials, setGprsCredentials] = useState(false);
  const [gprsUsername, setGprsUsername] = useState('');
  const [gprsPasswordSet, setGprsPasswordSet] = useState(false);
  const [changeGprsPassword, setChangeGprsPassword] = useState(false);
  const [gprsPassword, setGprsPassword] = useState('');
  const [gprsPasswordCheckEvent, setGprsPasswordCheckEvent] = useState(null);

  const handleDevicePassword = (value) => {
    setDevicePassword(value);
    if (devicePasswordCheckEvent != null) {
      handleDevicePasswordCheck(devicePasswordCheckEvent, value);
    }
  }

  const handleDevicePasswordCheck = (event, compare) => {
    setDevicePasswordCheckEvent(event);
    if (event.target.value !== compare) {
      event.target.setCustomValidity('Password fields must match!'); // ('⠀')
    } else {
      event.target.setCustomValidity('');
    }
  }

  const handleMqttPassword = (value) => {
    setMqttPassword(value);
    if (mqttPasswordCheckEvent != null) {
      handleMqttPasswordCheck(mqttPasswordCheckEvent, value);
    }
  }

  const handleMqttPasswordCheck = (event, compare) => {
    setMqttPasswordCheckEvent(event);
    if (event.target.value !== compare) {
      event.target.setCustomValidity('Password fields must match!'); // ('⠀')
    } else {
      event.target.setCustomValidity('');
    }
  }

  const handleGprsPassword = (value) => {
    setGprsPassword(value);
    if (gprsPasswordCheckEvent != null) {
      handleGprsPasswordCheck(gprsPasswordCheckEvent, value);
    }
  }

  const handleGprsPasswordCheck = (event, compare) => {
    setGprsPasswordCheckEvent(event);
    if (event.target.value !== compare) {
      event.target.setCustomValidity('Password fields must match!'); // ('⠀')
    } else {
      event.target.setCustomValidity('');
    }
  }

  const handleConfigFetch = async () => {
    try {
      setIsLoading(true);
      setLoadingHeader('Fetching configuration');
      setLoadingMessage('');
      const { data, status } = await Axios.get('/config');
      // const { data, status } = await Axios.get('https://sentinel.pasler.org/api'); // ?
      if(status === 200) {
        setWasConfigured(data.was_configured); // TODO
        setConfigMode(data.config_mode); // TODO
        setModuleType(data.module_type);
        setFirmwareVersion(data.firmware);
        // setHardwareVersion(data.hardware);
        setUnitId(data.unit_id);
        setMacAddress(data.mac_address);
        setHostname(data.hostname);
        setMqttServer(data.mqtt_server);
        setMqttPort(data.mqtt_port);
        setMqttCredentials(data.mqtt_credentials);
        setMqttUsername(data.mqtt_user);
        setMqttPasswordSet(data.mqtt_pass_set);
        setSslUse(data.ssl_use);
        setSslRootCertificate(data.ssl_root_ca);
        setSslValidateClient(data.ssl_validate_client);
        setSslClientCertificate(data.ssl_client_certificate);
        setSslClientPrivateKey(data.ssl_client_private_key);
        // setNtpServer(data.ntp); // ?
        setGnssMode(data.gnss_mode);
        setGprsApn(data.gprs_apn);
        setGprsCredentials(data.gprs_credentials);
        setGprsUsername(data.gprs_user);
        setGprsPasswordSet(data.gprs_pass_set);
        setIsLoading(false);
        setLoadingHeader('');
        setLoadingMessage('');
        setIsError(false);
        setErrorMessage('');
      } else {
        setIsLoading(false);
        setLoadingHeader('');
        setLoadingMessage('');
        setIsError(true);
        setErrorMessage('Fetch of device configuration was not successful!');
      }
    } catch (error) {
      setIsLoading(false);
      setLoadingHeader('');
      setLoadingMessage('');
      setIsError(true);
      setErrorMessage('Unable to fetch device configuration!');
    }
  }

  const sslRootCertificateRecalculateFontSize = () => {
    if (sslRootCertificateRef.current != null) {
      sslRootCertificateRef.current.style.fontSize = sslRootCertificateRef.current.scrollWidth / textAreaFontRatio + 'px';
    }
  }

  const sslClientCertificateRecalculateFontSize = () => {
    if (sslClientCertificateRef.current != null) {
      sslClientCertificateRef.current.style.fontSize = sslClientCertificateRef.current.scrollWidth / textAreaFontRatio + 'px';
    }
  }

  const sslClientPrivateKeyRecalculateFontSize = () => {
    if (sslClientPrivateKeyRef.current != null) {
      sslClientPrivateKeyRef.current.style.fontSize = sslClientPrivateKeyRef.current.scrollWidth / textAreaFontRatio + 'px';
    }
  }

  useEffect(() => {
    handleConfigFetch();
    window.addEventListener('resize', () => {
      sslRootCertificateRecalculateFontSize();
      sslClientCertificateRecalculateFontSize();
      sslClientPrivateKeyRecalculateFontSize();
    });
  }, []);

  const submitForm = async (event) => { // ?
    event.preventDefault();
    setIsLoading(true);
    setLoadingHeader('Saving configuration');
    setLoadingMessage('');
    let payload = {
      hostname: hostname,
      device_password_change: changePassword,
      device_password: devicePassword,
      mqtt_server: mqttServer,
      mqtt_port: mqttPort,
      mqtt_credentials: mqttCredentials,
      mqtt_username: mqttUsername,
      mqtt_password_change: changeMqttPassword,
      mqtt_password: mqttPassword,
      ssl_use: sslUse,
      ssl_root_ca: sslRootCertificate,
      ssl_validate_client: sslValidateClient,
      ssl_client_certificate: sslClientCertificate,
      ssl_client_private_key: sslClientPrivateKey,
      // ntp: ntpServer, // ?
      gnss_mode: gnssMode,
      gprs_apn: gprsApn,
      gprs_credentials: gprsCredentials,
      gprs_username: gprsUsername,
      gprs_password_change: changeGprsPassword,
      gprs_password: gprsPassword,
    }
    try {
      setLoadingMessage('Device will be rebooted');
      await Axios.post('/config', payload);
      setTimeout(() => {
        setLoadingMessage('You can close this window');
        window.top.close();
      }, 3000);
    } catch (error) {
      setIsLoading(false);
      setLoadingHeader('');
      setLoadingMessage('');
      setIsError(true);
      setErrorMessage('Error during saving configuration to device!');
    }
  }

  const loadingContent = () => {
    return (
      <Fragment>
        <p className='loader-header'>{loadingHeader}</p>
        <div className='loader'></div>
        <p className='loader-message'>{loadingMessage}</p>
      </Fragment>
    );
  }

  const errorContent = () => {
    return(
      <Fragment>
        <p className='error-header'>Something went wrong!</p>
        <div className='error-sign'>⚠</div>
        <p className='error-message'>{errorMessage}</p>
      </Fragment>
    );
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
              <td align='right' colSpan='1'><span id='type'>{moduleType}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Firmware version:</td>
              <td align='right' colSpan='1'><span id='firmware'>{firmwareVersion}</span></td>
            </tr>
            {/* <tr>
              <td align='left' colSpan='1'>Hardware version:</td>
              <td align='right' colSpan='1'><span id='hardware'>{hardwareVersion}</span></td>
            </tr> */}
            <tr>
              <td align='left' colSpan='1'>Unit ID:</td>
              <td align='right' colSpan='1'><span id='unit'>{unitId}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>MAC Address:</td>
              <td align='right' colSpan='1'><span id='mac'>{macAddress}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Configuration Mode:</td>
              <td align='right' colSpan='1'><span id='config'>{configMode ? 'YES' : 'NO'}</span></td>
            </tr>
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
                <td align='left' colSpan='9'>Hostname</td>
                <td align='right' colSpan='11'><input required={true} size='1' value={hostname} type='text' pattern='^[a-zA-Z\d]{0,}[a-zA-Z\d-]{0,}[a-zA-Z\d]{1,}$' /* pattern='^([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]{0,61}[a-zA-Z0-9])(\.([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]{0,61}[a-zA-Z0-9]))*$' */ maxLength='62' autoCapitalize='off' onChange={(event => setHostname(event.target.value))} /></td>
              </tr>
              <tr>
                <td align='left' colSpan='13'>Change Device Password</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' /*disabled={!configMode}*/ checked={changePassword} onChange={(event => setChangePassword(event.target.checked))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                changePassword ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Password</td>
                  <td align='right' colSpan='11'><input required={changePassword} size='1' value={devicePassword} type='password' autoCapitalize='off' onChange={(event => handleDevicePassword(event.target.value))} /></td>
                </tr>
                : null
              }
              {
                changePassword ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Password</td>
                  <td align='right' colSpan='11'><input required={changePassword} size='1' type='password' placeholder='Retype password' autoCapitalize='off' onChange={(event => handleDevicePasswordCheck(event, devicePassword))} /></td>
                </tr>
                : null
              }
              <tr>
                <td align='left' colSpan='9'>MQTT Server</td>
                <td align='center' colSpan='11'><input required={true} size='1' value={mqttServer} type='text' pattern='^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$' autoCapitalize='off' placeholder='mqtt.example.com' onChange={(event => setMqttServer(event.target.value))} /></td>
              </tr>
              <tr>
                <td align='left' colSpan='9'>MQTT Port</td>
                <td align='center' colSpan='11'><input required={true} size='1' value={mqttPort} type='text' pattern='^[0-9]{2,5}$' placeholder='1883' autoCapitalize='off' onChange={(event => setMqttPort(event.target.value))} /></td>
              </tr>
              <tr>
                <td align='left' colSpan='13'>MQTT Credentials</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' checked={mqttCredentials} onChange={(event) => setMqttCredentials(event.target.checked)} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                mqttCredentials ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Username</td>
                  <td align='center' colSpan='11'><input required={mqttCredentials} size='1' value={mqttUsername} type='text' autoCapitalize='off' onChange={(event => setMqttUsername(event.target.value))} /></td>
                </tr>
                : null
              }
              {
                mqttCredentials && !mqttPasswordSet ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Password</td>
                  <td align='center' colSpan='11'><input required={mqttCredentials && !mqttPasswordSet} size='1' value={mqttPassword} type='password' autoCapitalize='off' onChange={(event => handleMqttPassword(event.target.value))} /></td>
                </tr>
                : null
              }
              {
                mqttCredentials && !mqttPasswordSet ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Password</td>
                  <td align='center' colSpan='11'><input required={mqttCredentials && !mqttPasswordSet} size='1' type='password' placeholder='Retype password' autoCapitalize='off' onChange={(event => handleMqttPasswordCheck(event, mqttPassword))} /></td>
                </tr>
                : null
              }
              {
                mqttCredentials && mqttPasswordSet ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Change Password</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' checked={changeMqttPassword} onChange={(event => setChangeMqttPassword(event.target.checked))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                mqttCredentials && mqttPasswordSet && changeMqttPassword ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='7'>Password</td>
                  <td align='center' colSpan='11'><input required={mqttCredentials && mqttPasswordSet && changeMqttPassword} size='1' value={mqttPassword} type='password' autoCapitalize='off' onChange={(event => handleMqttPassword(event.target.value))} /></td>
                </tr>
                : null
              }
              {
                mqttCredentials && mqttPasswordSet && changeMqttPassword ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='7'>Password</td>
                  <td align='center' colSpan='11'><input required={mqttCredentials && mqttPasswordSet && changeMqttPassword} size='1' type='password' placeholder='Retype password' autoCapitalize='off' onChange={(event => handleMqttPasswordCheck(event, mqttPassword))} /></td>
                </tr>
                : null
              }
              <tr>
                <td align='left' colSpan='13'>TLS Communication</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' checked={sslUse} onChange={(event => setSslUse(event.target.checked))} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                sslUse ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='19' className='label'>Root CA Certificate</td>
                </tr>
                : null
              }
              {
                sslUse ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='center' colSpan='19'><textarea ref={setSslRootCertificateRef} required={sslUse} value={sslRootCertificate} autoCapitalize='off' placeholder='Paste here content of certificate file/s' onChange={(event => setSslRootCertificate(event.target.value))} /></td>
                </tr>
                : null
              }
              {
                sslUse ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Validate Client</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' checked={sslValidateClient} onChange={(event => setSslValidateClient(event.target.checked))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                sslUse && sslValidateClient ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='18' className='label'>Client Certificate</td>
                </tr>
                : null
              }
              {
                sslUse && sslValidateClient ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='center' colSpan='18'><textarea ref={setSslClientCertificateRef} required={sslUse && sslValidateClient} value={sslClientCertificate} autoCapitalize='off' placeholder='Paste here content of client certificate file' onChange={(event => setSslClientCertificate(event.target.value))} /></td>
                </tr>
                : null
              }
              {
                sslUse && sslValidateClient ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='18' className='label'>Client Private Key</td>
                </tr>
                : null
              }
              {
                sslUse && sslValidateClient ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='center' colSpan='18'><textarea ref={setSslClientPrivateKeyRef} required={sslUse && sslValidateClient} value={sslClientPrivateKey} autoCapitalize='off' placeholder='Paste here content of client certificate private key file' onChange={(event => setSslClientPrivateKey(event.target.value))} /></td>
                </tr>
                : null
              }
              <tr>
                <td align='left' style={{ lineHeight: '27px' }} colSpan='20'>GNSS Mode</td>
              </tr>
              <tr>
                <td align='center' colSpan='1'></td>
                <td align='right' colSpan='19'>
                  <select value={gnssMode} onChange={(event => setGnssMode(event.target.value))}>
                    <option value='1'>GLONASS</option>
                    <option value='2'>BEIDOU</option>
                    <option value='4'>GALILEO</option>
                    <option value='8'>QZSS</option>
                    <option value='3'>GLONASS + BEIDOU</option>
                    <option value='5'>GLONASS + GALILEO</option>
                    <option value='6'>BEIDOU + GALILEO</option>
                    <option value='7'>GLONASS + BEIDOU + GALILEO</option>
                    <option value='15'>GLONASS + BEIDOU + GALILEO + QZSS</option>
                  </select>
                </td>
              </tr>
              <tr>
                <td align='left' colSpan='9'>GPRS APN</td>
                <td align='center' colSpan='11'><input required={true} size='1' value={gprsApn} type='text' pattern='^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$' autoCapitalize='off' onChange={(event => setGprsApn(event.target.value))} /></td>
              </tr>
              <tr>
                <td align='left' colSpan='13'>GPRS Credentials</td>
                <td align='center' colSpan='7'>
                  <label className='switch'>
                    <input type='checkbox' checked={gprsCredentials} onChange={(event) => setGprsCredentials(event.target.checked)} />
                    <span className='slider'></span>
                  </label>
                </td>
              </tr>
              {
                gprsCredentials ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Username</td>
                  <td align='center' colSpan='11'><input required={gprsCredentials} size='1' value={gprsUsername} type='text' autoCapitalize='off' onChange={(event => setGprsUsername(event.target.value))} /></td>
                </tr>
                : null
              }
              {
                gprsCredentials && !gprsPasswordSet ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Password</td>
                  <td align='center' colSpan='11'><input required={gprsCredentials && !gprsPasswordSet} size='1' value={gprsPassword} type='password' autoCapitalize='off' onChange={(event => handleGprsPassword(event.target.value))} /></td>
                </tr>
                : null
              }
              {
                gprsCredentials && !gprsPasswordSet ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='8'>Password</td>
                  <td align='center' colSpan='11'><input required={gprsCredentials && !gprsPasswordSet} size='1' type='password' placeholder='Retype password' autoCapitalize='off' onChange={(event => handleGprsPasswordCheck(event, gprsPassword))} /></td>
                </tr>
                : null
              }
              {
                gprsCredentials && gprsPasswordSet ?
                <tr>
                  <td align='center' colSpan='1'></td>
                  <td align='left' colSpan='12'>Change Password</td>
                  <td align='center' colSpan='7'>
                    <label className='switch'>
                      <input type='checkbox' checked={changeGprsPassword} onChange={(event => setChangeGprsPassword(event.target.checked))} />
                      <span className='slider'></span>
                    </label>
                  </td>
                </tr>
                : null
              }
              {
                gprsCredentials && gprsPasswordSet && changeGprsPassword ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='7'>Password</td>
                  <td align='center' colSpan='11'><input required={gprsCredentials && gprsPasswordSet && changeGprsPassword} size='1' value={gprsPassword} type='password' autoCapitalize='off' onChange={(event => handleGprsPassword(event.target.value))} /></td>
                </tr>
                : null
              }
              {
                gprsCredentials && gprsPasswordSet && changeGprsPassword ?
                <tr>
                  <td align='center' colSpan='2'></td>
                  <td align='left' colSpan='7'>Password</td>
                  <td align='center' colSpan='11'><input required={gprsCredentials && gprsPasswordSet && changeGprsPassword} size='1' type='password' placeholder='Retype password' autoCapitalize='off' onChange={(event => handleGprsPasswordCheck(event, gprsPassword))} /></td>
                </tr>
                : null
              }
            </tbody>
          </table>
          <input type='submit' value='Save & Reboot' />
          <h2 className='info'>A reboot is required for the changes to take effect after modifying the configuration</h2>
        </form>
      </Fragment>
    );
  }

  return (
    <div className='app'>
      <center className='container'>
        <div className='content'>
          <h1 className='header'>Configuration</h1>
          { isLoading ? loadingContent() : isError ? errorContent() : content() }
        </div>
      </center>
    </div>
  );
}

export default App;
