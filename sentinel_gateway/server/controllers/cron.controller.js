import { CronJob } from 'cron';
import ldap from './ldap.controller.js';

const timezone = Intl.DateTimeFormat().resolvedOptions().timeZone;

var _ldapUserSyncCronJob = new CronJob( // run every day at 1:30
  '0 30 1 * * *', // '0 30 1 * * *',
	ldap.allUserSync,
	null,
	false,
	timezone
);

function cronSetup() {
  _ldapUserSyncCronJob.start();
};

export {
  cronSetup,
  _ldapUserSyncCronJob
};
