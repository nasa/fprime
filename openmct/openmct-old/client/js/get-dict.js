// Get dictionary
function getDictionary() {
  // Needs directory from root of application
  return http.get('/server/res/dictionary.json').then(function (result) {
    return result.data;
  });
}
