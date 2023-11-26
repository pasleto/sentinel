import React, { useState, useEffect, Fragment } from 'react';
import Axios from 'axios';

const itemsPerPage = 5;

function LogsPage(props) {

  const [isLoading, setIsLoading] = useState(false);
  const [pageNumber, setPageNumber] = useState(1);
  const [pages, setPages] = useState(0);
  const [count, setCount] = useState(0);
  const [data, setData] = useState([]);

  const removeFromDatabase = async (index) => {
    let payload = {
      id: data[index].id,
    }
    try {
      await Axios.post('/log/remove', payload);
      await handleCountFetch(true);
      await handleStorageUsageFetch();
    } catch (error) {
      console.error(error); // ?
    }
  }

  const handleStorageUsageFetch = async () => {
    try {
      const { data, status } = await Axios.get(`/storage`);
      if (status === 200) {
        props.setConfig((prevState) => ({...prevState, storage_usage: data.percentage}));
      }
    } catch (error) {
      console.error(error); // ?
    }
  }

  const handleDataFetch = async (page) => {
    setIsLoading(true);
    try {
      const { data, status } = await Axios.get(`/log/data?page=${page}`);
      // const { data, status } = await Axios.get('https://sentinel.pasler.org/api');
      if (status === 200) {
        setData(data.data);
        setIsLoading(false);
      } else {
        setIsLoading(false);
      }
    } catch (error) {
      setIsLoading(false);
    }
  }

  const handleCountFetch = async (dataFetch) => {
    if (dataFetch) setIsLoading(true);
    try {
      const { data, status } = await Axios.get('/log/count');
      // const { data, status } = await Axios.get('https://sentinel.pasler.org/api');
      if (status === 200) {
        setCount(data.count);
        if (data.count > 0) {
          var pagesCount = Math.ceil(data.count/itemsPerPage);
          setPages(pagesCount);
          if (dataFetch) {
            var page = pageNumber <= pagesCount ? pageNumber : pagesCount;
            setPageNumber(page);
            await handleDataFetch(page);
          }
        } else {
          setData([]);
          setPages(0);
          setPageNumber(1);
          if (dataFetch) setIsLoading(false);
        }
      } else {
        setCount(0);
        setData([]);
        setPages(0);
        setPageNumber(1);
        if (dataFetch) setIsLoading(false);
      }
    } catch (error) {
      setCount(0);
      setData([]);
      setPages(0);
      setPageNumber(0);
      if (dataFetch) setIsLoading(false);
    }
  }

  useEffect(() => {
    handleCountFetch(true);
  }, []);

  const changePage = async (page) => {
    setPageNumber(page);
    await handleDataFetch(page);
  }

  const paginationPagesList = () => {
    var options = [];
    for (let i = 1; i <= pages; i++) {
      options.push(<option value={i}>{i}</option>);
    }
    return options;
  }

  const pagination = () => {
    if (pages > 0) {
      return (
        <td align='center' colSpan='20'>
          <input type='button' className='pagination-button' disabled={!(pageNumber > 1)} value='<<' onClick={() => changePage(1)} />
          <input type='button' className='pagination-button' disabled={!(pageNumber > 1)} value='<' onClick={() => changePage(pageNumber-1)} />
          <select style={{width: '65px', margin: '4px 8px'}} value={pageNumber} onChange={(e) => changePage(+e.target.value)}>
            { paginationPagesList() }
          </select>
          <input type='button' className='pagination-button' disabled={!(pageNumber < pages)} value='>' onClick={() => changePage(pageNumber+1)} />
          <input type='button' className='pagination-button' disabled={!(pageNumber < pages)} value='>>' onClick={() => changePage(pages)} />
        </td>
      );
    } else {
      return null;
    }
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
              <td align='left' colSpan='1'>Number of records:</td>
              <td align='right' colSpan='1'><span id='count'>{count}</span></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Storage Usage:</td>
              <td align='right' colSpan='1'><span id='storage'>{props.config.storage_usage} % <meter align='left' className='storage-meter' min={0} low={69} high={80} max={100} value={props.config.storage_usage} ></meter></span></td>
            </tr>
            <tr>
              <td align='center' colSpan='2'>&nbsp;</td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Status Page:</td>
              <td align='right' colSpan='1'><input type='button' className='link-button' value='➔' /*value='➤'*/ onClick={() => props.setComponentRender('status')} /></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Configuration Page:</td>
              <td align='right' colSpan='1'><input type='button' className='link-button' value='➔' /*value='➤'*/ onClick={() => props.setComponentRender('config')} /></td>
            </tr>
          </tbody>
        </table>
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
              <td align='center' colSpan='18'>&nbsp;</td>
              <td align='center' colSpan='2'>&nbsp;</td>
            </tr>
            <tr>
              <td align='center' colSpan='20'><hr/></td>
            </tr>
            {
              isLoading ?
              <tr>
                <td align='center' colSpan='20'><div className='loader'></div></td>
              </tr>
              :
              data.length > 0 ? data.map((record, index) => {
                return (
                  <tr key={index}>
                    <td align='center' colSpan='18'>
                      <div>
                        <div style={{display: 'flex', flexDirection: 'row'}}>
                          <input style={{textAlign: 'center', marginRight: '1px', fontSize: 'smaller', width: '42%'}} type='text' readOnly={true} value={new Date(record.timestamp * 1000).toLocaleString()} onChange={null} />
                          <input style={{textAlign: 'center', marginLeft: '1px', fontSize: 'smaller', width: '58%'}} type='text' readOnly={true} value={record.initiator} onChange={null} />
                        </div>
                        <div style={{display: 'flex', flexDirection: 'row'}}>
                          <input style={{textAlign: 'center', marginRight: '1px', fontSize: 'smaller', width: '42%'}} type='text' readOnly={true} value={record.state} onChange={null} />
                          <input style={{textAlign: 'center', marginLeft: '1px', fontSize: 'smaller', width: '58%'}} type='text' readOnly={true} value={record.measurement} onChange={null} />
                        </div>
                        <div style={{display: 'flex', flexDirection: 'row'}}>
                          <input style={{textAlign: 'center', fontSize: 'smaller', width: '100%'}} type='text' readOnly={true} value={record.status} onChange={null} />
                        </div>
                      </div>
                    </td>
                    <td align='center' colSpan='2'>
                      <div>
                        <input type='button' value={'❌'} onClick={() => removeFromDatabase(index)} />
                      </div>
                    </td>
                  </tr>
                );
              })
              :
              <tr>
                <td align='center' colSpan='20'><div className='database-no-data'>No Data</div></td>
              </tr>
            }
            <tr>
              <td align='center' colSpan='20'><hr/></td>
            </tr>
            <tr>
              { pagination() }
            </tr>
            <tr>
              <td align='center' colSpan='20'><hr/></td>
            </tr>
          </tbody>
        </table>
        <h2 className='info'>&nbsp;</h2>
      </Fragment>
    );
  }

  return (
    <div className='content'>
      <h1 className='header'>Logs</h1>
      { props.isLoading ? props.loadingContent() : props.isError ? props.errorContent() : content() }
    </div>
  );
}

export default LogsPage;