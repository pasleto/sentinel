import React, { useState, useEffect, Fragment } from 'react';
import useWebSocket/*, { ReadyState }*/ from 'react-use-websocket';
import StatusPage from './StatusPage';
import ConfigPage from './ConfigPage';
import DatabasePage from './DatabasePage';
import LogsPage from './LogsPage';

const textAreaFontRatio = 40.82355;
const minValidityDate = 946684800; // 00:00 01/01/2000 UTC
const logItemsPerPage = 5;
const dbItemsPerPage = 5;

function App() {
  const padTo2Digits = (num) => {
    return num.toString().padStart(2, '0');
  }
  const timestampToDate = (timestamp) => {
    var date = new Date(timestamp * 1000);
    return (
      [
        date.getFullYear(),
        padTo2Digits(date.getMonth() + 1),
        padTo2Digits(date.getDate()),
      ].join('-') +
      ' ' +
      [
        padTo2Digits(date.getHours()),
        padTo2Digits(date.getMinutes()),
        // padTo2Digits(date.getSeconds()),
      ].join(':')
    );
  }
  const dateToTimestamp = (date) => {
    var out = new Date(date);
    const unixTimestamp = Math.floor(out.getTime() / 1000);
    return unixTimestamp;
  }
  const [socketUrl] = useState(`ws://${window.location.hostname}/ws`);
  const [isLoading, setIsLoading] = useState(true);
  const [loadingHeader, setLoadingHeader] = useState('Connecting');
  const [loadingMessage, setLoadingMessage] = useState('');
  const [isError, setIsError] = useState(false);
  const [errorMessage, setErrorMessage] = useState('');
  const [connectionAccessPoint, setConnectionAccessPoint] = useState(false); // !
  const [connectionEthernet, setConnectionEthernet] = useState(false); // !
  const [config, setConfig] = useState(null);
  const [currentlyStandaloneMode, setCurrentlyStandaloneMode] = useState(false);
  const [componentRender, setComponentRender] = useState('status');
  // const [configLoading, configSetLoading] = useState(false); // TODO
  const [logIsLoading, logSetIsLoading] = useState(false);
  const [logRemoveLoading, logSetRemoveLoading] = useState(false);
  const [logPageNumber, logSetPageNumber] = useState(1);
  const [logPages, logSetPages] = useState(0);
  const [logCount, logSetCount] = useState(0);
  const [logData, logSetData] = useState([]);
  const [dbIsLoading, dbSetIsLoading] = useState(false);
  const [dbRemoveLoading, dbSetRemoveLoading] = useState(false);
  const [dbAddLoading, dbSetAddLoading] = useState(false);
  const [dbUpdateLoading, dbSetUpdateLoading] = useState(false);
  const [dbPageNumber, dbSetPageNumber] = useState(1);
  const [dbPages, dbSetPages] = useState(0);
  const [dbCount, dbSetCount] = useState(0);
  const [dbData, dbSetData] = useState([]);
  const [dbNewRecordId, dbSetNewRecordId] = useState('');
  const [dbNewRecordValue, dbSetNewRecordValue] = useState('');
  const [dbNewRecordValidity, dbSetNewRecordValidity] = useState(false);
  const [dbNewRecordValidSince, dbSetNewRecordValidSince] = useState(minValidityDate);
  const [dbNewRecordValidUntil, dbSetNewRecordValidUntil] = useState(minValidityDate);

  const dbSetNewRecordDefaults = () => {
    dbSetNewRecordId('');
    dbSetNewRecordValue('');
    dbSetNewRecordValidity(false);
    dbSetNewRecordValidSince(minValidityDate);
    dbSetNewRecordValidUntil(minValidityDate);
  }

  const { sendJsonMessage/*, readyState*/ } = useWebSocket(socketUrl, {
    onMessage: (e) => handleIncomingMessage(e.data),
    onOpen: (e) => handleSocketOpen(e),
    onError: (e) => handleSocketError(e),
    onClose: (e) => handleSocketClose(e),
    shouldReconnect: (e) => true,
    // reconnectAttempts: 10,
    // reconnectInterval: 10000,
  });

  // const connectionStatus = {
  //   [ReadyState.CONNECTING]: 'Connecting',
  //   [ReadyState.OPEN]: 'Open',
  //   [ReadyState.CLOSING]: 'Closing',
  //   [ReadyState.CLOSED]: 'Closed',
  //   [ReadyState.UNINSTANTIATED]: 'Uninstantiated',
  // }[readyState];

  const handleSocketOpen = (e) => {
    console.log('WebSocket: Open');
    setIsLoading(true);
    setLoadingHeader('Fetching configuration');
    setLoadingMessage('');
    setIsError(false);
    setErrorMessage('');
    sendJsonMessage({
      topic: 'CONFIG-GET-REQ'
    });
    setComponentRender('status');
  }

  const handleSocketError = (e) => {
    console.log('WebSocket: Error');
    console.error(e);
    setIsLoading(false);
    setLoadingHeader('');
    setLoadingMessage('');
    setIsError(true);
    setErrorMessage('Connection Error');
  }

  const handleSocketClose = (e) => {
    console.log('WebSocket: Closed');
    console.log(e);
    setIsLoading(false);
    setLoadingHeader('');
    setLoadingMessage('');
    setIsError(true);
    setErrorMessage('Connection Closed');
  }

  const storageUsageFetch = () => {
    sendJsonMessage({
      topic: 'STORAGE-USAGE-REQ'
    });
  }

  const logCountFetch = (dataFetch) => {
    if (dataFetch) logSetIsLoading(true);
    sendJsonMessage({
      topic: 'LOG-COUNT-REQ',
      init: dataFetch
    });
  }

  const logDataFetch = (page) => {
    logSetIsLoading(true);
    sendJsonMessage({
      topic: 'LOG-DATA-REQ',
      page: page
    });
  }

  const dbCountFetch = (dataFetch) => {
    if (dataFetch) dbSetIsLoading(true);
    sendJsonMessage({
      topic: 'DB-COUNT-REQ',
      init: dataFetch
    });
  }

  const dbDataFetch = (page) => {
    dbSetIsLoading(true);
    sendJsonMessage({
      topic: 'DB-DATA-REQ',
      page: page
    });
  }

  const handleIncomingMessage = (inc) => { // TODO
    // try {
    var decoded = JSON.parse(inc);
    console.log('WebSocket: Incoming Message'); // !
    console.log(`TOPIC - ${decoded.topic}:`); // !
    console.log(decoded); // !
    switch (decoded.topic) {
      case "CONNECTION-TYPE-RES":
        setConnectionAccessPoint(decoded.accesspoint);
        setConnectionEthernet(decoded.ethernet);
        break;
      case "CONFIG-GET-RES":
        if (!(Object.keys(decoded.data).length === 0)) {
          setConfig(decoded.data);
          setCurrentlyStandaloneMode(decoded.data.standalone_mode);
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
          setErrorMessage('Error during fetching configuration! Try to refresh!');
        }
      break;
      case "CONFIG-SET-RES": // !
        // ! modify to reflect changes on device -> if reboot is needed or not, etc.
        if (!decoded.data.error) {
          if (decoded.data.reboot) {
            setLoadingMessage('Device will be rebooted');
            setTimeout(() => {
              setLoadingMessage('You can close this window');
              window.top.close();
            }, 3000);
          } else {
            setIsLoading(false);
            setLoadingHeader('');
            setLoadingMessage('');
          }
        } else {
          setIsLoading(false);
          setLoadingHeader('');
          setLoadingMessage('');
          setIsError(true);
          setErrorMessage('Error during saving configuration to device!');
        }
        break;
      case "STORAGE-USAGE-RES": // ?
        if (decoded.data.percentage !== null && decoded.data.percentage !== undefined) {
          setConfig((prevState) => ({...prevState, storage_usage: decoded.data.percentage}));
        } else {
          // ?
        }
        break;
      case "LOG-COUNT-RES": // ?
        if (!decoded.data.error) {
          logSetCount(decoded.data.count);
          if (decoded.data.count > 0) {
            var logPagesCount = Math.ceil(decoded.data.count/logItemsPerPage);
            logSetPages(logPagesCount);
            if (decoded.data.init) {
              var logPage = logPageNumber <= logPagesCount ? logPageNumber : logPagesCount;
              logSetPageNumber(logPage);
              sendJsonMessage({
                topic: 'LOG-DATA-REQ',
                page: logPage
              });
            }
          } else {
            logSetData([]);
            logSetPages(0);
            logSetPageNumber(1);
            if (decoded.data.init) logSetIsLoading(false);
          }
        } else {
          // ? notify
          logSetCount(0);
          logSetData([]);
          logSetPages(0);
          logSetPageNumber(1);
          if (decoded.data.init) logSetIsLoading(false);
        }
        break;
      case "LOG-DATA-RES":
        logSetData(decoded.data);
        logSetIsLoading(false);
        break;
      case "LOG-REMOVE-RES": // ?
        logSetRemoveLoading(false);
        if (!decoded.data.error) {
          logCountFetch(true);
          storageUsageFetch();
        } else {
          // ? notify
        }
        break;
      case "LOG-REMOVE-ALL-RES":
        logSetRemoveLoading(false);
        if (!decoded.data.error) {
          logCountFetch(true);
          storageUsageFetch();
        } else {
          // ? notify
        }
        break;
      case "DB-COUNT-RES": // ?
        if (!decoded.data.error) {
          dbSetCount(decoded.data.count);
          if (decoded.data.count > 0) {
            var dbPagesCount = Math.ceil(decoded.data.count/dbItemsPerPage);
            dbSetPages(dbPagesCount);
            if (decoded.data.init) {
              var dbPage = dbPageNumber <= dbPagesCount ? dbPageNumber : dbPagesCount;
              dbSetPageNumber(dbPage);
              sendJsonMessage({
                topic: 'DB-DATA-REQ',
                page: dbPage
              });
            }
          } else {
            dbSetData([]);
            dbSetPages(0);
            dbSetPageNumber(1);
            if (decoded.data.init) dbSetIsLoading(false);
          }
        } else {
          // ? notify
          dbSetCount(0);
          dbSetData([]);
          dbSetPages(0);
          dbSetPageNumber(1);
          if (decoded.data.init) dbSetIsLoading(false);
        }
        break;
      case "DB-DATA-RES":
        dbSetData(decoded.data);
        dbSetIsLoading(false);
        break;
      case "DB-ADD-RES": // ?
        dbSetAddLoading(false);
        dbSetUpdateLoading(false);
        if (!decoded.data.error) {
          dbCountFetch(true);
          storageUsageFetch();
          if(decoded.data.new) dbSetNewRecordDefaults();
        } else {
          // ? - notify
          if(decoded.data.new) dbSetNewRecordDefaults();
        }
        break;
      case "DB-REMOVE-RES": // ?
        dbSetRemoveLoading(false);
        if (!decoded.data.error) {
          dbCountFetch(true);
          storageUsageFetch();
        } else {
          // ? - notify
        }
        break;
      case "DB-REMOVE-ALL-RES": // ?
        dbSetRemoveLoading(false);
        if (!decoded.data.error) {
          dbCountFetch(true);
          storageUsageFetch();
        } else {
          // ? - notify
        }
        break;
      case "REBOOT-STATUS-RES": // !
        // !
        break;
      case "ERROR-RES": // !
        // !
        break;
      default:
        break;
    }
    // } catch (error) {
    //   console.log("handleIncomingMessage error: ");
    //   console.log(e);
    // }
  }

  // const socketConnectingContent = () => {
  //   return (
  //     <Fragment>
  //       <div className='content' /* style={{display: 'flex', flexDirection: 'column', height: '100vh', justifyContent: 'center', alignItems: 'center', padding: '0', margin: '0'}} */>
  //         <h1 className='header'>&nbsp;</h1>
  //         <p className='loader-header'>Connection:</p>
  //           <div className='loader'></div>
  //         <p className='loader-message'>{connectionStatus}</p>
  //       </div>
  //     </Fragment>
  //   );
  // }

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
        <div className='error-sign'>⚠️</div>
        <p className='error-message'>{errorMessage}</p>
      </Fragment>
    );
  }

  const handleComponentRender = () => {
    switch (componentRender) {
      case 'config':
        return <ConfigPage
          textAreaFontRatio={textAreaFontRatio}
          sendJsonMessage={sendJsonMessage}
          setComponentRender={setComponentRender}
          config={config}
          setConfig={setConfig}
          currentlyStandaloneMode={currentlyStandaloneMode}
          isLoading={isLoading}
          setIsError={setIsError}
          setIsLoading={setIsLoading}
          setLoadingMessage={setLoadingMessage}
          isError={isError}
          setLoadingHeader={setLoadingHeader}
          setErrorMessage={setErrorMessage}
          loadingContent={loadingContent}
          errorContent={errorContent}
        />;
      case 'database':
        return <DatabasePage
          itemsPerPage={dbItemsPerPage}
          dbIsLoading={dbIsLoading}
          dbSetIsLoading={dbSetIsLoading}
          removeLoading={dbRemoveLoading}
          setRemoveLoading={dbSetRemoveLoading}
          addLoading={dbAddLoading}
          setAddLoading={dbSetAddLoading}
          updateLoading={dbUpdateLoading}
          setUpdateLoading={dbSetUpdateLoading}
          pageNumber={dbPageNumber}
          setPageNumber={dbSetPageNumber}
          pages={dbPages}
          setPages={dbSetPages}
          count={dbCount}
          setCount={dbSetCount}
          data={dbData}
          setData={dbSetData}
          minValidityDate={minValidityDate}
          timestampToDate={timestampToDate}
          dateToTimestamp={dateToTimestamp}
          setNewRecordDefaults={dbSetNewRecordDefaults}
          newRecordId={dbNewRecordId}
          setNewRecordId={dbSetNewRecordId}
          newRecordValue={dbNewRecordValue}
          setNewRecordValue={dbSetNewRecordValue}
          newRecordValidity={dbNewRecordValidity}
          setNewRecordValidity={dbSetNewRecordValidity}
          newRecordValidSince={dbNewRecordValidSince}
          setNewRecordValidSince={dbSetNewRecordValidSince}
          newRecordValidUntil={dbNewRecordValidUntil}
          setNewRecordValidUntil={dbSetNewRecordValidUntil}
          setComponentRender={setComponentRender}
          sendJsonMessage={sendJsonMessage}
          countFetch={dbCountFetch}
          dataFetch={dbDataFetch}
          config={config}
          setConfig={setConfig}
          currentlyStandaloneMode={currentlyStandaloneMode}
          isLoading={isLoading} setIsError={setIsError}
          setIsLoading={setIsLoading}
          setLoadingMessage={setLoadingMessage}
          isError={isError}
          setLoadingHeader={setLoadingHeader}
          setErrorMessage={setErrorMessage}
          loadingContent={loadingContent}
          errorContent={errorContent}
        />;
      case 'logs':
        return <LogsPage
          itemsPerPage={logItemsPerPage}
          logIsLoading={logIsLoading}
          removeLoading={logRemoveLoading}
          setRemoveLoading={logSetRemoveLoading}
          logSetIsLoading={logSetIsLoading}
          pageNumber={logPageNumber}
          setPageNumber={logSetPageNumber}
          pages={logPages}
          setPages={logSetPages}
          count={logCount}
          setCount={logSetCount}
          data={logData}
          setData={logSetData}
          sendJsonMessage={sendJsonMessage}
          countFetch={logCountFetch}
          dataFetch={logDataFetch}
          setComponentRender={setComponentRender}
          config={config}
          setConfig={setConfig}
          currentlyStandaloneMode={currentlyStandaloneMode}
          isLoading={isLoading}
          setIsError={setIsError}
          setIsLoading={setIsLoading}
          setLoadingMessage={setLoadingMessage}
          isError={isError}
          setLoadingHeader={setLoadingHeader}
          setErrorMessage={setErrorMessage}
          loadingContent={loadingContent}
          errorContent={errorContent}
        />;
      case 'status':
      default:
        return <StatusPage
          sendJsonMessage={sendJsonMessage}
          setComponentRender={setComponentRender}
          config={config}
          setConfig={setConfig}
          currentlyStandaloneMode={currentlyStandaloneMode}
          isLoading={isLoading} setIsError={setIsError}
          setIsLoading={setIsLoading}
          setLoadingMessage={setLoadingMessage}
          isError={isError}
          setLoadingHeader={setLoadingHeader}
          setErrorMessage={setErrorMessage}
          loadingContent={loadingContent}
          errorContent={errorContent}
        />;
    }
  }

  useEffect(() => {
    window.addEventListener('beforeunload', (event) => {
      event.preventDefault();
      sendJsonMessage({
        topic: 'CLOSE-CONNECTION-REQ',
      });
      return true;
    });
  }, []);

  // ? readyState !== ReadyState.OPEN ? socketConnectingContent() : handleComponentRender()
  return (
    <div className='app'>
      <center className='container'>
        { handleComponentRender() }
      </center>
    </div>
  );

}

export default App;
