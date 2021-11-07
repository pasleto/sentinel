const _api_routes = [
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '/api',
    method: 'GET',
    description: 'General API message',
    request_body: {}
  },
  {
    route: '/api/auth/local',
    method: 'POST',
    description: 'Authentication using local database credentials',
    request_body: {
      username: 'String [mandatory]',
      password: 'String [mandatory]'
    }
  },
  {
    route: '/api/auth/ldap',
    method: 'POST',
    description: 'Authentication using active directory credentials',
    request_body: {
      username: 'String [mandatory]',
      password: 'String [mandatory]'
    }
  },
  {
    route: '/api/auth/windows',
    method: 'POST',
    description: 'Passwordless authentication using active directory account name',
    request_body: {
      username: 'String [mandatory]'
    }
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
  {
    route: '',
    method: '',
    description: '',
    request_body: {}
  },
];

const api_message = {
  application: 'Sentinel',
  description: 'Sentinel',
  author: {
    name: 'Tomáš Pásler',
    nickname: 'pasleto',
    web: 'https:/pasleto.eu/'
  },
  api_routes: _api_routes
};

module.exports = {
  api_message
};
