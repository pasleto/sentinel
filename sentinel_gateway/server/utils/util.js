const stripAccents = (function () {
  var inChrs = 'àáâãäçèéêëěěìíîïñòóôõöùúûüýÿÀÁÂÃÄÇÈÉÊËÌÍÎÏÑÒÓÔÕÖÙÚÛÜÝřŘšŠťŤďĎňŇĚÉžŽčČ ';
  var outChrs = 'aaaaaceeeeeeiiiinooooouuuuyyAAAAACEEEEIIIINOOOOOUUUUYrRsStTdDnNEEzZcC_';
  var charsRgx = new RegExp('[' + inChrs + ']', 'g');
  var transl = {};
  var i;
  var lookup = function (m) { return transl[m] || m; };

  for (i = 0; i < inChrs.length; i++) {
    transl[ inChrs[i] ] = outChrs[i];
  }

  return function (string) { return string.replace(charsRgx, lookup); };
})();

export {
  stripAccents
};
