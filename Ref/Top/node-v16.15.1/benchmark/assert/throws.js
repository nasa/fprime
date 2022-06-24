'use strict';

const common = require('../common.js');
const { throws, doesNotThrow } = require('assert');

const bench = common.createBenchmark(main, {
  n: [1e4],
  method: [ 'doesNotThrow', 'throws_TypeError', 'throws_RegExp' ],
});

function main({ n, method }) {
  const throwError = () => { throw new TypeError('foobar'); };
  const doNotThrowError = () => { return 'foobar'; };
  const regExp = /foobar/;
  const message = 'failure';

  switch (method) {
    case 'doesNotThrow':
      bench.start();
      for (let i = 0; i < n; ++i) {
        doesNotThrow(doNotThrowError);
      }
      bench.end(n);
      break;
    case 'throws_TypeError':
      bench.start();
      for (let i = 0; i < n; ++i) {
        throws(throwError, TypeError, message);
      }
      bench.end(n);
      break;
    case 'throws_RegExp':
      bench.start();
      for (let i = 0; i < n; ++i) {
        throws(throwError, regExp, message);
      }
      bench.end(n);
      break;
    default:
      throw new Error(`Unsupported method ${method}`);
  }
}
