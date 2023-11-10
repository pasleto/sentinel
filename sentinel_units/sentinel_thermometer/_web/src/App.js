import React, { useState, useEffect, Fragment } from 'react';
import Axios from 'axios';
import StatusPage from './StatusPage';
import ConfigPage from './ConfigPage';
import LogsPage from './LogsPage';

function App() {
  const [isLoading, setIsLoading] = useState(true);
  const [loadingHeader, setLoadingHeader] = useState('');
  const [loadingMessage, setLoadingMessage] = useState('');
  const [isError, setIsError] = useState(false);
  const [errorMessage, setErrorMessage] = useState('');
  const [config, setConfig] = useState(null);
  const [currentlyStandaloneMode, setCurrentlyStandaloneMode] = useState(false);
  const [componentRender, setComponentRender] = useState('status');

  const handleConfigFetch = async () => {
    try {
      setIsLoading(true);
      setLoadingHeader('Fetching configuration');
      setLoadingMessage('');
      const { data, status } = await Axios.get('/config');
      // const { data, status } = await Axios.get('https://sentinel.pasler.org/api'); // ?
      if(status === 200) {
        setConfig(data);
        setCurrentlyStandaloneMode(data.standalone_mode);
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

  useEffect(() => {
    handleConfigFetch();
  }, []);

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
      case 'status':
        return <StatusPage setComponentRender={setComponentRender} config={config} setConfig={setConfig} currentlyStandaloneMode={currentlyStandaloneMode} isLoading={isLoading} setIsError={setIsError} setIsLoading={setIsLoading} setLoadingMessage={setLoadingMessage} isError={isError} setLoadingHeader={setLoadingHeader} setErrorMessage={setErrorMessage} loadingContent={loadingContent} errorContent={errorContent} />;
      case 'config':
        return <ConfigPage setComponentRender={setComponentRender} config={config} setConfig={setConfig} currentlyStandaloneMode={currentlyStandaloneMode} isLoading={isLoading} setIsError={setIsError} setIsLoading={setIsLoading} setLoadingMessage={setLoadingMessage} isError={isError} setLoadingHeader={setLoadingHeader} setErrorMessage={setErrorMessage} loadingContent={loadingContent} errorContent={errorContent} />;
      case 'logs':
        return <LogsPage setComponentRender={setComponentRender} config={config} setConfig={setConfig} currentlyStandaloneMode={currentlyStandaloneMode} isLoading={isLoading} setIsError={setIsError} setIsLoading={setIsLoading} setLoadingMessage={setLoadingMessage} isError={isError} setLoadingHeader={setLoadingHeader} setErrorMessage={setErrorMessage} loadingContent={loadingContent} errorContent={errorContent} />;
      default:
        return <StatusPage setComponentRender={setComponentRender} config={config} setConfig={setConfig} currentlyStandaloneMode={currentlyStandaloneMode} isLoading={isLoading} setIsError={setIsError} setIsLoading={setIsLoading} setLoadingMessage={setLoadingMessage} isError={isError} setLoadingHeader={setLoadingHeader} setErrorMessage={setErrorMessage} loadingContent={loadingContent} errorContent={errorContent} />;
    }
  }

  return (
    <div className='app'>
      <center className='container'>
        { handleComponentRender() }
      </center>
    </div>
  );

}

export default App;
