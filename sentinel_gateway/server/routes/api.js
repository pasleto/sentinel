import express from 'express';
import { apiLoggerHandler } from '../controllers/logger.controller.js';
import * as middleware from '../controllers/middleware.controller.js';
import jwt from '../controllers/jwt.controller.js';

const router = express.Router();
router.use(apiLoggerHandler);
router.get('/', (req, res) => {
  res.status(200).send({
    application: 'Sentinel',
    description: 'Sentinel',
    author: {
      name: 'Tomáš Pásler',
      nickname: 'pasleto',
      web: 'https:/pasleto.eu/'
    }
  });
});

router.get('/test', (req, res) => {
  res.status(200).send({
    status: 'OK', 
    data: {
      message: 'TEST GET'
    }
  });
}); 

router.post('/test', (req, res) => {
  res.status(200).send({
    status: 'OK', 
    data: {
      message: 'TEST POST'
    }
  });
}); 

router.post('/client-app/login-password', async (req, res) => {
  if (req.body.username && req.body.password) {
    await jwt.clientAppPasswordAuthenticate(req.body.username, req.body.password, (result) => {
      if (result.status == 'OK') {
        return res.status(200).send(result);
      } else {
        return res.status(403).send(result);
      }
    });
  } else {
    return res.status(403).send({
      status: 'NOK', 
      data: {
        message: 'No credentials passed!'
      }
    });
  }
}); 

router.post('/material-storage/login-password', async (req, res) => { // TODO
  var authheader = req.headers.authorization;
  if (!authheader) {
    return res.status(403).send({
      status: 'NOK', 
      data: {
        message: 'No authorization passed!'
      }
    });
  }
  var credentials = new Buffer.from(authheader.split(' ')[1],'base64').toString().split(':');

  // await jwt.materialStoragePasswordAuthenticate(credentials[0], credentials[1], (result) => {
  //   console.log(result);
  //   if (result.status == 'OK') {
  //     return res.status(200).send(result);
  //   } else {
  //     return res.status(403).send(result);
  //   }
  // });

  res.status(200).send('TODO')
});

router.post('/material-storage/login-card', async (req, res) => { // TODO
  var authheader = req.headers.authorization;
  if (!authheader) {
    return res.status(403).send({
      status: 'NOK', 
      data: {
        message: 'No authorization passed!'
      }
    });
  }
  // var token = new Buffer.from(authheader.split(' ')[1],'base64').toString().split(':');
  // var user = token[0];
  // var pass = token[1];

  // await jwt.materialStorageCardAuthenticate(card, (result) => {
  //   console.log(result);
  //   if (result.status == 'OK') {
  //     return res.status(200).send(result);
  //   } else {
  //     return res.status(403).send(result);
  //   }
  // });

  res.status(200).send('TODO')
});

router.post('/material-storage/login-barcode', async (req, res) => { // TODO
  var authheader = req.headers.authorization;
  if (!authheader) {
    return res.status(403).send({
      status: 'NOK', 
      data: {
        message: 'No authorization passed!'
      }
    });
  }
  // var token = new Buffer.from(authheader.split(' ')[1],'base64').toString().split(':');
  // var user = token[0];
  // var pass = token[1];

  // await jwt.materialStorageCardBarcodeAuthenticate(crypted_card, (result) => {
  //   console.log(result);
  //   if (result.status == 'OK') {
  //     return res.status(200).send(result);
  //   } else {
  //     return res.status(403).send(result);
  //   }
  // });

  res.status(200).send('TODO')
});

router.use(middleware.notFoundHandler);
router.use(middleware.errorHandler);

export default router;
