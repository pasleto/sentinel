const error = require('../../../helpers/error');
const ldap = require('../../../helpers/ldap');

async function authenticateLocal({username, password}) {
  /*
  Authentication using local database credentials
  */
  if (!username) throw new error.PropertyRequiredError('Username');
  if (!password) throw new error.PropertyRequiredError('Password');

  console.log(username);
  console.log(password);

  // return "Success";

}

async function authenticateLdap({username, password}) {
  /*
  Authentication using active directory credentials
  */
  if (!username) throw new error.PropertyRequiredError('Username');
  if (!password) throw new error.PropertyRequiredError('Password');

  // check if user exist in active directory
  // if yes
  // authenticate user against active directory, find user in database, create user return object
  // if no
  // throw error

}

async function authenticateWindows({username}) {
  /*
  Authentication using active directory username - passwordless
  */
  if (!username) throw new error.PropertyRequiredError('Username');

  // check if user exist in active directory
  // if yes
  // find user in database, create user return object
  // if no
  // throw error

}

module.exports = {
  authenticateLocal,
  authenticateLdap,
  authenticateWindows
};