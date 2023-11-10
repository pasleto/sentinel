import React, { useEffect, Fragment } from 'react';

function DatabasePage(props) {
  const addToDatabase = async () => {
    props.setAddLoading(true);
    props.sendJsonMessage({
      topic: 'DB-ADD-REQ',
      new: true,
      id: props.newRecordId,
      value: props.newRecordValue,
      has_validity: props.newRecordValidity,
      valid_since: props.newRecordValidity ? props.newRecordValidSince : null,  // data to timestamp
      valid_until: props.newRecordValidity ? props.newRecordValidUntil : null   // data to timestamp
    });
  }

  const removeFromDatabase = (index) => {
    props.setRemoveLoading(true);
    props.sendJsonMessage({
      topic: 'DB-REMOVE-REQ',
      id: props.data[index].id,
      value: props.data[index].value
    });
  }

  const removeAllFromDatabase = () => {
    props.setRemoveLoading(true);
    props.sendJsonMessage({
      topic: 'DB-REMOVE-ALL-REQ'
    });
  }

  const updateInDatabase = async (index) => {  // ?       // ? - what is non changing value ?? -> if changing id and value together -> new card instead -> how to solve ??
    props.setUpdateLoading(true);
    props.sendJsonMessage({
      topic: 'DB-ADD-REQ',
      new: false,
      id: props.data[index].id,
      value: props.data[index].value,
      has_validity: props.data[index].has_validity,
      valid_since: props.data[index].has_validity ? props.data[index].valid_since : null,
      valid_until: props.data[index].has_validity ? props.data[index].valid_until : null
    });
  }

  const wasRecordModified = (index) => {
    if (props.data[index].was_modified !== null && props.data[index].was_modified !== undefined && props.data[index].was_modified) {
      return true;
    } else {
      return false;
    }
  }

  const updateRecordId = (index, value) => {
    if (props.data[index].id_modified !== null && props.data[index].id_modified !== undefined && props.data[index].id_modified) {
      const tempData = [...props.data];
      tempData[index].id = value;
      props.setData(tempData);
    } else {
      const tempData = [...props.data];
      tempData[index].id_original = tempData[index].id;
      tempData[index].id = value;
      tempData[index].id_modified = true;
      tempData[index].was_modified = true;
      props.setData(tempData);
    }
  }

  const updateRecordValue = (index, value) => {
    if (props.data[index].value_modified !== null && props.data[index].value_modified !== undefined && props.data[index].value_modified) {
      const tempData = [...props.data];
      tempData[index].value = value;
      props.setData(tempData);
    } else {
      const tempData = [...props.data];
      tempData[index].value_original = tempData[index].value;
      tempData[index].value = value;
      tempData[index].value_modified = true;
      tempData[index].was_modified = true;
      props.setData(tempData);
    }
  }

  const updateRecordValidity = (index, value) => {
    if (props.data[index].has_validity_modified !== null && props.data[index].has_validity_modified !== undefined && props.data[index].has_validity_modified) {
      const tempData = [...props.data];
      tempData[index].has_validity = value;
      props.setData(tempData);
    } else {
      const tempData = [...props.data];
      tempData[index].has_validity_original = tempData[index].has_validity;
      tempData[index].has_validity = value;
      tempData[index].has_validity_modified = true;
      tempData[index].was_modified = true;
      props.setData(tempData);
    }
  }

  const updateRecordValiditySince = (index, value) => {
    if (props.data[index].valid_since_modified !== null && props.data[index].valid_since_modified !== undefined && props.data[index].valid_since_modified) {
      const tempData = [...props.data];
      tempData[index].valid_since = value;
      props.setData(tempData);
    } else {
      const tempData = [...props.data];
      tempData[index].valid_since_original = tempData[index].valid_since;
      tempData[index].valid_since = value;
      tempData[index].valid_since_modified = true;
      tempData[index].was_modified = true;
      props.setData(tempData);
    }
  }

  const updateRecordValidityUntil = (index, value) => {
    if (props.data[index].valid_until_modified !== null && props.data[index].valid_until_modified !== undefined && props.data[index].valid_until_modified) {
      const tempData = [...props.data];
      tempData[index].valid_until = value;
      props.setData(tempData);
    } else {
      const tempData = [...props.data];
      tempData[index].valid_until_original = tempData[index].valid_until;
      tempData[index].valid_until = value;
      tempData[index].valid_until_modified = true;
      tempData[index].was_modified = true;
      props.setData(tempData);
    }
  }

  const canAddToDatabase = () => {
    if (props.newRecordId.length > 0 && props.newRecordValue.length > 0) {
      if (props.newRecordValidity) {
        if (props.newRecordValidSince >= props.minValidityDate && props.newRecordValidUntil >= props.minValidityDate) {
          if (props.newRecordValidSince < props.newRecordValidUntil) {
            return true;
          } else {
            return false;
          }
        } else {
          return false;
        }
      } else {
        return true;
      }
    } else {
      return false;
    }
  }

  const canUpdateRecordInDatabase = (index) => {
    if (wasRecordModified(index)) {
      if (props.data[index].id.length > 0 && props.data[index].value.length > 0) {
        if (props.data[index].has_validity) {
          if (props.data[index].valid_since >= props.minValidityDate && props.data[index].valid_until >= props.minValidityDate) {
            if (props.data[index].valid_since < props.data[index].valid_until) {
              return true;
            } else {
              return false;
            }
          } else {
            return false;
          }
        } else {
          return true;
        }
      } else {
        return false;
      }
    }
  }

  const returnModifiedRecord = (index) => {
    const tempData = [...props.data];
    if (tempData[index].id_modified) {
      tempData[index].id = tempData[index].id_original;
      delete tempData[index].id_original;
      delete tempData[index].id_modified;
    }
    if (tempData[index].value_modified) {
      tempData[index].value = tempData[index].value_original;
      delete tempData[index].value_original;
      delete tempData[index].value_modified;
    }
    if (tempData[index].has_validity_modified) {
      tempData[index].has_validity = tempData[index].has_validity_original;
      delete tempData[index].has_validity_original;
      delete tempData[index].has_validity_modified;
    }
    if (tempData[index].valid_since_modified) {
      tempData[index].valid_since = tempData[index].valid_since_original;
      delete tempData[index].valid_since_original;
      delete tempData[index].valid_since_modified;
    }
    if (tempData[index].valid_until_modified) {
      tempData[index].valid_until = tempData[index].valid_until_original;
      delete tempData[index].valid_until_original;
      delete tempData[index].valid_until_modified;
    }
    if (tempData[index].was_modified) {
      delete tempData[index].was_modified;
    }
    props.setData(tempData);
  }

  const handleRecordRemoveAction = (index) => {
    if (wasRecordModified(index)) {
      returnModifiedRecord(index);
    } else {
      removeFromDatabase(index);
    }
  }

  useEffect(() => {
    props.dbSetIsLoading(false);
    props.setRemoveLoading(false);
    props.setAddLoading(false);
    props.setUpdateLoading(false);
    props.setPageNumber(1);
    props.setPages(0);
    props.setCount(0);
    props.setData([]);
    props.setNewRecordDefaults();
    props.countFetch(true);
  }, []);

  const changePage = async (page) => {
    props.setPageNumber(page);
    props.dataFetch(page);
  }

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
          <input type='button' className='pagination-button' disabled={(props.updateLoading || props.removeLoading || props.addLoading) ? true : !(props.pageNumber > 1)} value='<<' onClick={() => changePage(1)} />
          <input type='button' className='pagination-button' disabled={(props.updateLoading || props.removeLoading || props.addLoading) ? true : !(props.pageNumber > 1)} value='<' onClick={() => changePage(props.pageNumber-1)} />
          <select style={{width: '65px', margin: '4px 8px'}} disabled={props.updateLoading || props.removeLoading || props.addLoading} value={props.pageNumber} onChange={(e) => changePage(+e.target.value)}>
            { paginationPagesList() }
          </select>
          <input type='button' className='pagination-button' disabled={(props.updateLoading || props.removeLoading || props.addLoading) ? true : !(props.pageNumber < props.pages)} value='>' onClick={() => changePage(props.pageNumber+1)} />
          <input type='button' className='pagination-button' disabled={(props.updateLoading || props.removeLoading || props.addLoading) ? true : !(props.pageNumber < props.pages)} value='>>' onClick={() => changePage(props.pages)} />
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
              <td align='right' colSpan='1'><input type='button' disabled={props.updateLoading || props.removeLoading || props.addLoading} className='link-button' value='➔' onClick={() => props.setComponentRender('status')} /></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Configuration Page:</td>
              <td align='right' colSpan='1'><input type='button' disabled={props.updateLoading || props.removeLoading || props.addLoading} className='link-button' value='➔' onClick={() => props.setComponentRender('config')} /></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Logs Page:</td>
              <td align='right' colSpan='1'><input type='button' disabled={props.updateLoading || props.removeLoading || props.addLoading} className='link-button' value='➔' onClick={() => props.setComponentRender('logs')} /></td>
            </tr>
            {/* <tr>
              <td align='center' colSpan='2'>&nbsp;</td>
            </tr>
            <tr>
              <td align='center' colSpan='2'>XXX</td>
            </tr> */}
            <tr>
              <td align='center' colSpan='2'>&nbsp;</td>
            </tr>
            <tr>
              <td align='center' colSpan='2'>ID and VALUE columns have UNIQUE constraint! If either of them is reused, the old record containing the duplicate value will be replaced by the new one.</td>
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
              <td align='center' colSpan='7'>&nbsp;</td>
              <td align='center' colSpan='7'>&nbsp;</td>
              <td align='center' colSpan='3'>&nbsp;</td>
              <td align='center' colSpan='3'>&nbsp;</td>
            </tr>

            <tr>
              <td align='center' colSpan='20'><hr/></td>
            </tr>
            <tr>
              <td align='left' colSpan='18'>Remove all records:</td>
              <td align='right' colSpan='2'>
                <input type='button' disabled={!(props.data.length > 0) || props.updateLoading || props.removeLoading || props.addLoading} value={'❌'} onClick={() => {if(window.confirm('Are sure want to delete all records?')){removeAllFromDatabase()};}} />
              </td>
            </tr>
            
            <tr>
              <td align='center' colSpan='20'><hr/></td>
            </tr>
            <tr>
              <td align='center' colSpan='20'>Add new record:</td>
            </tr>
            <tr>
              <td align='center' colSpan='7'>
                <div>
                  <input style={{textAlign: 'center'}} placeholder='ID / LABEL' required={true} disabled={props.updateLoading || props.removeLoading || props.addLoading} value={props.newRecordId} type='text' onChange={(e) => props.setNewRecordId(e.target.value)} />
                  <input type='datetime-local' disabled={(props.updateLoading || props.removeLoading || props.addLoading) ? true : !props.newRecordValidity} value={props.timestampToDate(props.newRecordValidSince)} min={props.timestampToDate(props.minValidityDate)} onChange={(e) => props.setNewRecordValidSince(props.dateToTimestamp(e.target.value))} />
                </div>
              </td>
              <td align='center' colSpan='7'>
                <div>
                  <input style={{textAlign: 'center'}} placeholder='VALUE' required={true} disabled={props.updateLoading || props.removeLoading || props.addLoading} value={props.newRecordValue} type='text' onChange={(e) => props.setNewRecordValue(e.target.value)} />
                  <input type='datetime-local' disabled={(props.updateLoading || props.removeLoading || props.addLoading) ? true : !props.newRecordValidity} value={props.timestampToDate(props.newRecordValidUntil)} min={props.timestampToDate(props.minValidityDate)} onChange={(e) => props.setNewRecordValidUntil(props.dateToTimestamp(e.target.value))} />
                </div>
              </td>
              <td align='center' colSpan='3'>
                <label className='switch'>
                  <input type='checkbox' disabled={props.updateLoading || props.removeLoading || props.addLoading} checked={props.newRecordValidity} onChange={(e) => props.setNewRecordValidity(e.target.checked)} />
                  <span className='slider'></span>
                </label>
              </td>
              <td align='center' colSpan='3'>
                <div>
                  <input type='button' disabled={(props.updateLoading || props.removeLoading || props.addLoading) ? true : !canAddToDatabase()} value={'✔️'} onClick={() => addToDatabase()} />
                  <input type='button' disabled={props.updateLoading || props.removeLoading || props.addLoading} value={'✖️'} onClick={() => props.setNewRecordDefaults()} />
                </div>
              </td>
            </tr>
            <tr>
              <td align='center' colSpan='20'><hr/></td>
            </tr>
            {
              props.dbIsLoading ?
              <tr>
                <td align='center' colSpan='20'><div className='loader'></div></td>
              </tr>
              :
              props.data.length > 0 ? props.data.map((record, index) => {
                return (
                  <tr key={index}>
                    <td align='center' colSpan='7'>
                      <div>
                        <input style={{textAlign: 'center'}} type='text' required={true} disabled={props.updateLoading || props.removeLoading || props.addLoading} value={record.id} onChange={(e) => updateRecordId(index, e.target.value)} />
                        <input type='datetime-local' disabled={(props.updateLoading || props.removeLoading || props.addLoading) ? true : !record.has_validity} min={props.timestampToDate(props.minValidityDate)} value={record.valid_since !== null ? props.timestampToDate(record.valid_since) : props.timestampToDate(props.minValidityDate)} onChange={(e) => updateRecordValiditySince(index, props.dateToTimestamp(e.target.value))} />
                      </div>
                    </td>
                    <td align='center' colSpan='7'>
                      <div>
                        <input style={{textAlign: 'center'}} type='text' required={true} disabled={props.updateLoading || props.removeLoading || props.addLoading} value={record.value} onChange={(e) => updateRecordValue(index, e.target.value)} />
                        <input type='datetime-local' disabled={(props.updateLoading || props.removeLoading || props.addLoading) ? true : !record.has_validity} min={props.timestampToDate(props.minValidityDate)} value={record.valid_until !== null ? props.timestampToDate(record.valid_until) : props.timestampToDate(props.minValidityDate)} onChange={(e) => updateRecordValidityUntil(index, props.dateToTimestamp(e.target.value))} />
                      </div>
                    </td>
                    <td align='center' colSpan='3'>
                      <label className='switch'>
                        <input type='checkbox' disabled={props.updateLoading || props.removeLoading || props.addLoading} checked={record.has_validity} onChange={(e) => updateRecordValidity(index, e.target.checked)} />
                        <span className='slider'></span>
                      </label>
                    </td>
                    <td align='center' colSpan='3'>
                      <div>
                        <input type='button' disabled={(props.updateLoading || props.removeLoading || props.addLoading) ? true : !canUpdateRecordInDatabase(index)} value={'✔️'} onClick={() => updateInDatabase(index)} />
                        <input type='button' disabled={props.updateLoading || props.removeLoading || props.addLoading} value={wasRecordModified(index) ? '✖️' : '❌'} onClick={() => handleRecordRemoveAction(index)} />
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
      <h1 className='header'>Database</h1>
      { props.isLoading ? props.loadingContent() : props.isError ? props.errorContent() : content() }
    </div>
  );
}

export default DatabasePage;
