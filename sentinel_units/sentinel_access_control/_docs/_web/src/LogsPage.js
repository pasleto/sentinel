import React, { useEffect, Fragment } from 'react';

function LogsPage(props) {
  const removeFromDatabase = (index) => {
    props.setRemoveLoading(true);
    props.sendJsonMessage({
      topic: 'LOG-REMOVE-REQ',
      id: props.data[index].id
    });
  }

  const removeAllFromDatabase = () => {
    props.setRemoveLoading(true);
    props.sendJsonMessage({
      topic: 'LOG-REMOVE-ALL-REQ'
    });
  }

  const changePage = async (page) => {
    props.setPageNumber(page);
    props.dataFetch(page);
  }

  useEffect(() => {
    props.logSetIsLoading(false);
    props.setRemoveLoading(false);
    props.setPageNumber(1);
    props.setPages(0);
    props.setCount(0);
    props.setData([]);
    props.countFetch(true);
  }, []);

  const paginationPagesList = () => {
    var options = [];
    for (let i = 1; i <= props.pages; i++) {
      options.push(<option value={i}>{i}</option>);
    }
    return options;
  }

  const pagination = () => {
    if (props.pages > 0) {
      return (
        <td align='center' colSpan='20'>
          <input type='button' className='pagination-button' disabled={props.removeLoading ? true : !(props.pageNumber > 1)} value='<<' onClick={() => changePage(1)} />
          <input type='button' className='pagination-button' disabled={props.removeLoading ? true : !(props.pageNumber > 1)} value='<' onClick={() => changePage(props.pageNumber-1)} />
          <select style={{width: '65px', margin: '4px 8px'}} disabled={props.removeLoading} value={props.pageNumber} onChange={(e) => changePage(+e.target.value)}>
            { paginationPagesList() }
          </select>
          <input type='button' className='pagination-button' disabled={props.removeLoading ? true : !(props.pageNumber < props.pages)} value='>' onClick={() => changePage(props.pageNumber+1)} />
          <input type='button' className='pagination-button' disabled={props.removeLoading ? true : !(props.pageNumber < props.pages)} value='>>' onClick={() => changePage(props.pages)} />
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
              <td align='right' colSpan='1'><span id='count'>{props.count}</span></td>
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
              <td align='right' colSpan='1'><input type='button' disabled={props.removeLoading} className='link-button' value='➔' onClick={() => props.setComponentRender('status')} /></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Configuration Page:</td>
              <td align='right' colSpan='1'><input type='button' disabled={props.removeLoading} className='link-button' value='➔' onClick={() => props.setComponentRender('config')} /></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Database Page:</td>
              <td align='right' colSpan='1'><input type='button' disabled={props.removeLoading} className='link-button' value='➔' onClick={() => props.setComponentRender('database')} /></td>
            </tr>
            {/* <tr>
              <td align='center' colSpan='2'>&nbsp;</td>
            </tr>
            <tr>
              <td align='center' colSpan='2'>XXX</td>
            </tr> */}
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
            <tr>
              <td align='left' colSpan='18'>Remove all records:</td>
              <td align='right' colSpan='2'>
                <input type='button' disabled={!(props.data.length > 0) || props.removeLoading} value={'❌'} onClick={() => {if(window.confirm('Are sure want to delete all records?')){removeAllFromDatabase()};}} />
              </td>
            </tr>
            
            <tr>
              <td align='center' colSpan='20'><hr/></td>
            </tr>
            {
              props.logIsLoading ?
              <tr>
                <td align='center' colSpan='20'><div className='loader'></div></td>
              </tr>
              :
              props.data.length > 0 ? props.data.map((record, index) => {
                return (
                  <tr key={index}>
                    <td align='center' colSpan='18'>
                      <div>
                        <div style={{display: 'flex', flexDirection: 'row'}}>
                          <input style={{textAlign: 'center', marginRight: '1px', fontSize: 'smaller', width: '42%'}} type='text' disabled={props.removeLoading} readOnly={true} value={new Date(record.timestamp * 1000).toLocaleString()} onChange={null} />
                          <input style={{textAlign: 'center', marginLeft: '1px', fontSize: 'smaller', width: '58%'}} type='text' disabled={props.removeLoading} readOnly={true} value={record.initiator} onChange={null} />
                        </div>
                        <div style={{display: 'flex', flexDirection: 'row'}}>
                          <input style={{textAlign: 'center', marginRight: '1px', fontSize: 'smaller', width: '42%'}} type='text' disabled={props.removeLoading} readOnly={true} value={record.state} onChange={null} />
                          <input style={{textAlign: 'center', marginLeft: '1px', fontSize: 'smaller', width: '58%'}} type='text' disabled={props.removeLoading} readOnly={true} value={record.card_value} onChange={null} />
                        </div>
                        <div style={{display: 'flex', flexDirection: 'row'}}>
                          <input style={{textAlign: 'center', fontSize: 'smaller', width: '100%'}} type='text' disabled={props.removeLoading} readOnly={true} value={record.status} onChange={null} />
                        </div>
                      </div>
                    </td>
                    <td align='center' colSpan='2'>
                      <div>
                        <input type='button' disabled={props.removeLoading} value={'❌'} onClick={() => removeFromDatabase(index)} />
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
            {
              props.pages > 0 ?
                <tr>
                  <td align='center' colSpan='20'><hr/></td>
                </tr>
              : null
            }
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
