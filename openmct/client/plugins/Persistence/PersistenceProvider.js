/**
  * PersistenceProvider.js
  *
  * Implements the interface for a legacy persistence plugin in OpenMCT, using
  * the embedded LevelDB database implemented on the server
  *
  * @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
  */


/**
  * @constructor
  * @implements {PersistenceService}
  * @param {string} baseURL The URL of the server hosting the embedded database
  * @param {string} space (Optional) The name of the persistence space to use. Defaults to
  *                       'mct' if none is provided.
  */
function PersistenceProvider(baseURL, space) {
    // 'mct' appears to be the only persistence space OpenMCT actually ever
    // tries to use
    this.spaces = space ? [space] : ['mct']
    this.baseURL = baseURL;
}

/**
  * Create a new object in the persistence store
  * @param {string} space The persistence space to use
  * @param {string} key Key to use to identify object in database
  * @param {string} value Domain object to set value to
  * @return {Promise} Promise which resolves true if the operation succeeded
  */
PersistenceProvider.prototype.createObject = function (space, key, value) {
    return new Promise( (resolve, reject) => {
        this.makeHTTPRequest('PUT', '/persistence/objects/' + key, value)
            .then( (response) => resolve(true) )
            .catch( (err) => reject(err) );
    });
};

/**
  * Get the value of an object from the persistence store
  * @param {string} space The persistence space to use
  * @param {string} key Key of object in database
  * @return {Promise} Promise which resolves with the domain object, or undefined
  *                   if the object wasn't found
  */
PersistenceProvider.prototype.readObject = function (space, key) {
    return new Promise( (resolve, reject) => {
        this.makeHTTPRequest('GET', '/persistence/objects/' + key)
            .then( (response) => resolve(JSON.parse(response.value)) )
            .catch( (err) => resolve(undefined) );
    });
};

/**
  * Update value of an object in the persistence store
  * @param {string} space The persistence space to use
  * @param {string} key Key of object in database
  * @param {string} value Domain object to set value to
  * @return {Promise} Promise which resolves true if the operation succeeded
  */
PersistenceProvider.prototype.updateObject = PersistenceProvider.prototype.createObject;

/**
  * Delete an object from the persistence store
  * @param {string} space The persistence space to use
  * @param {string} key Key to use to identify object in database
  * @return {Promise} Promise which resolves true if the operation succeeded
  */
PersistenceProvider.prototype.deleteObject = function (space, key) {
  return new Promise( (resolve, reject) => {
      this.makeHTTPRequest('DELETE', '/persistence/objects/' + key)
          .then( (response) => resolve(response.value) )
          .catch( (err) => reject(err) );
  });
};

/**
  * List available persistence spaces
  * @return {Promise} Promise which resolves with an array containing the names of
  * all available persistence spaces
  */
PersistenceProvider.prototype.listSpaces = function () {
    return new Promise( (resolve, reject) => {
        resolve(this.spaces);
    });
};

/**
  * List all objects in the database
  * @return {Promise} Promise which resolves with an array containing the keys
  * of all objects stored in the database
  */
PersistenceProvider.prototype.listObjects = function () {
  return new Promise( (resolve, reject) => {
      this.makeHTTPRequest('GET', '/persistence/list/objects')
          .then( (response) => resolve(JSON.parse(response.value)) )
          .catch( (err) => reject(new Error('Could not get list of objects')) );
  });
};

/**
  * Makes an HTTP request with the given method and value to the URL of the persistence
  * backed passed in during instantiation.
  * @param {String} method The HTTP method to use (GET, POST, ...)
  * @param {String} route The endpoint of the persitence backed to make a request to
  * @param {Value} value (Optional) The body of the request, if it needs one
  * @return {Promise} A promise that resolves with the response body JSON as a JS Object
  *                   if the request succeeds, or rejects with the error message
  *                   if it fails
  */
PersistenceProvider.prototype.makeHTTPRequest = function (method, route, value) {
    var httpRequest = new XMLHttpRequest(),
        baseURL = this.baseURL;

    return new Promise( (resolve, reject) => {
        httpRequest.onreadystatechange = function () {
            if (httpRequest.readyState === XMLHttpRequest.DONE) {
                if (httpRequest.status >= 200 && httpRequest.status < 300) {
                    let responseObj = JSON.parse(httpRequest.responseText);
                    resolve(responseObj);
                } else {
                    reject(httpRequest.responseText);
                }
            }
        }

        httpRequest.open(method, baseURL + route);
        if (value) {
            httpRequest.setRequestHeader('Content-Type', 'application/json');
            httpRequest.send(JSON.stringify(value));
        } else {
            httpRequest.send();
        }
    });
};
