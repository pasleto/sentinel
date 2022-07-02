import ldapScopeInit from './ldap/ldap.init.js';
import printScopeInit from './print/print.init.js';
import smtpScopeInit from './smtp/smtp.init.js';

async function settingModelInit() {
  ldapScopeInit();
  printScopeInit();
  smtpScopeInit();

  // TODO
   
};

export default settingModelInit;
