const ActiveDirectory = require('activedirectory');
const ad_config = {
  url: process.env.LDAP_URL,
  baseDN: process.env.LDAP_BASE_DN,
  username: process.env.LDAP_USERNAME,
  password: process.env.LDAP_PASSWORD,
  attributes: process.env.LDAP_USER_ATTRIBUTES_ARRAY.split(',')
}
const ad = new ActiveDirectory(ad_config);

// ldap calls to find || authenticate user
// ldap calls to replicate ldap user data to database object


async function findUser(username) {
  /*
  Find user in active directory
  */

}

async function authenticateUser(username, password) {
  /*
  Authentication user against active directory
  */

}


module.exports = {
  findUser,
  authenticateUser
};