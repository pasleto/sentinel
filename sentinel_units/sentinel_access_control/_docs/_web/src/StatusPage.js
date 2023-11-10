import React, { Fragment } from 'react';

function StatusPage(props) {

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
              <td align='left' colSpan='1'>Configuration Page:</td>
              <td align='right' colSpan='1'><input type='button' className='link-button' value='➔' onClick={() => props.setComponentRender('config')} /></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Database Page:</td>
              <td align='right' colSpan='1'><input type='button' className='link-button' value='➔' onClick={() => props.setComponentRender('database')} /></td>
            </tr>
            <tr>
              <td align='left' colSpan='1'>Logs Page:</td>
              <td align='right' colSpan='1'><input type='button' className='link-button' value='➔' onClick={() => props.setComponentRender('logs')} /></td>
            </tr>
            <tr>
              <td align='center' colSpan='2'>&nbsp;</td>
            </tr>
            <tr>
              <td align='center' colSpan='2'>Database actions are memory intensive. This can affect device functionality during performing those actions!</td>
            </tr>
            <tr>
              <td align='center' colSpan='2'>&nbsp;</td>
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
              <td align='center' colSpan='20'><hr/></td>
            </tr>
            
            <tr>
              <td align='left' colSpan='10'>Hostname</td>
              <td align='center' colSpan='10'><span style={{ lineHeight: '27px' }}>{props.config.hostname}</span></td>
            </tr>
           
            <tr>
              <td align='center' colSpan='20'><hr/></td>
            </tr>
           
            <tr>
              <td align='right' colSpan='5'>
                <input type='button' /*disabled={}*/ value={'✔️'} onClick={() => alert('TODO - PASS')} />
              </td>
              <td align='right' colSpan='5'>
                <input type='button' /*disabled={}*/ value={'🔐'} onClick={() => alert('TODO - BLOCK')} />
              </td>
              <td align='right' colSpan='5'>
                <input type='button' /*disabled={}*/ value={'🔓'} onClick={() => alert('TODO - BYPASS')} />
              </td>
              <td align='right' colSpan='5'>
                <input type='button' /*disabled={}*/ value={'🔄'} onClick={() => alert('TODO - IDLE')} />
              </td>
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
      <h1 className='header'>Status</h1>
      { props.isLoading ? props.loadingContent() : props.isError ? props.errorContent() : content() }
    </div>
  );
}

export default StatusPage;
