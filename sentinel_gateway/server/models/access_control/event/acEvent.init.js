const acEventInitData = [
  {
    'event_id': 1,
    'initiator_type': 'RTC',
    'friendly_name': 'RTC Lost Power',
    'status': 'error',
    'description': 'Battery might be dead',
    'is_protected': true
  },
  {
    'event_id': 2,
    'initiator_type': 'Door Sensor',
    'friendly_name': 'Door Closed',
    'status': 'info',
    'description': 'Door has been closed',
    'is_protected': true
  },
  {
    'event_id': 3,
    'initiator_type': 'Door Sensor',
    'friendly_name': 'Door Opened',
    'status': 'info',
    'description': 'Door has been opened',
    'is_protected': true
  },
  {
    'event_id': 4,
    'initiator_type': 'Reader',
    'friendly_name': 'Valid Card',
    'status': 'success',
    'description': 'Valid card has been presented to reader',
    'is_protected': true
  },
  {
    'event_id': 5,
    'initiator_type': 'Reader',
    'friendly_name': 'Invalid Card',
    'status': 'error',
    'description': 'Invalid card has been presented to reader',
    'is_protected': true
  },
  {
    'event_id': 6,
    'initiator_type': 'Button',
    'friendly_name': 'Button Triggered',
    'status': 'success',
    'description': 'Button has been triggered',
    'is_protected': true
  }
];

export default acEventInitData;