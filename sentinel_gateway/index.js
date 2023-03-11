import * as dotenv from 'dotenv';
dotenv.config();
import pjson from './package.json' assert { type: 'json' };
import logSymbols from 'log-symbols';
import app from './server/controllers/app.controller.js';

console.log(logSymbols.info, `----------------------------------------------------------------------------------------------------`);
console.log(logSymbols.info, `[${pjson.name}] ${pjson.description} - Version: ${pjson.version} | ${pjson.author.name} (${pjson.author.url})`);
console.log(logSymbols.info, `----------------------------------------------------------------------------------------------------`);

await app.init();
