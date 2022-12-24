import express from 'express';
import { resolve } from 'path';
const router = express.Router();

router.get('/favicon.ico', (req, res) => {
  res.sendFile(resolve('client/favicon.ico'));
});

router.get('/api-call/favicon.ico', (req, res) =>  {
  res.sendFile(resolve('client/favicon.ico'));
});

router.post('/api-call/test', (req, res) => {
  if (global.sentinel != null) {
    if (req.body.unit) {
      global.sentinel.emit('api-call-test-post', {
        unit_id: req.body.unit,
        data: req.body.data
      }, (ack) => {
  
        if (ack.status == 'OK') {
          return res.status(200).send();
        } else {
          return res.status(403).send();
        }
  
      });
    } else {
      res.status(403).send({
        status: 'NOK',
        data: {
          message: 'No unit ID provided'
        }
      });
    }
  } else {
    res.status(500).send({
      status: 'NOK',
      data: {
        message: 'Remote server is not connected!'
      }
    });
  }
});

router.get('/api-call/*', (req, res) =>  {
  if (global.sentinel != null) {
    global.sentinel.emit('api-call-get', {
      path: req.path.replace("/api-call",""),
      headers: req.headers,
      body: req.body
    }, (ack) => {

      try {
        res.status(ack.status).setHeader('content-type', ack.contentType).send(ack.data); // TODO
      } catch (error) {
        try {
          res.send(ack.data);
        } catch (error) {
          res.status(500).send({
            status: 'NOK',
            data: {
              message: 'Unknown error!'
            }
          });
        }
      }

    });
  } else {
    res.status(500).send({
      status: 'NOK',
      data: {
        message: 'Remote server is not connected!'
      }
    });
  }
});

router.post('/api-call/*', (req, res) => {
  if (global.sentinel != null) {
    global.sentinel.emit('api-call-post', {
      path: req.path.replace("/api-call",""),
      headers: req.headers,
      body: req.body
    }, (ack) => {
      // if (ack.status == 'OK') {
      //   return res.status(200).send(ack);
      // } else {
      //   return res.status(403).send(ack);
      // }

      // res.status(ack.status).setHeader('content-type', ack.contentType).send(ack.data); // TODO

      try {
        res.status(ack.status).setHeader('content-type', ack.contentType).send(ack.data); // TODO
      } catch (error) {
        try {
          res.send(ack.data);
        } catch (error) {
          res.status(500).send({
            status: 'NOK',
            data: {
              message: 'Unknown error!'
            }
          });
        }
      }

    });
  } else {
    res.status(500).send({
      status: 'NOK',
      data: {
        message: 'Remote server is not connected!'
      }
    });
  }
});

router.get('*', (req, res) => {
  res.status(404).send();
});

export default router;
