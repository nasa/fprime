var fs = require('fs');

var level = require('level');


/**
 * Manage LevelDB databases.  Supports fetching the database that applies to a
 * given date; supports readstream queries accross multiple databases, and
 * supports automatic closing of unused databases and pruning of older
 * databases.
 */
function DatabaseManager(options) {
    if (!fs.existsSync(options.databasePath)) {
        fs.mkdirSync(options.databasePath);
    }

    this.dbPath = options.databasePath;
    if (options.hasOwnProperty('daysToKeep') && options.daysToKeep !== 0) {
        console.log(`LevelDB: History will keep latest ${options.daysToKeep} day(s) of data.`);
        console.log(`LevelDB: History will be pruned every ${options.pruneInterval/60/60/1000} hour(s).`);
        this.daysToKeep = options.daysToKeep;
        setInterval(this.pruneDatabases.bind(this), options.pruneInterval); // Hourly.
    } else {
        console.log(`LevelDB: History will not be automatically pruned by server.`);
    }
    this.cache = {};
    this.lastUsed = {};

    setInterval(this.closeUnusedDatabases.bind(this), 5000);
};

/**
 * Returns true of database exists.
 * @private
 */
DatabaseManager.prototype.dbExists = function (dbKey) {
    return this.cache[dbKey] || fs.existsSync(this.getDbPath(dbKey));
};

/**
 * Get path to database, given key.
 * @private
 */
DatabaseManager.prototype.getDbPath = function (dbKey) {
    return this.dbPath + '/' + dbKey;
};

// Remove all files in a directory and then remove directory.
function rmr(directory) {
    return new Promise(function (resolve, reject) {
        fs.readdir(directory, function (error, files) {
            if (error) {
                return reject(error);
            }
            Promise.all(files.map(function (file) {
                return new Promise(function (res, rej) {
                    fs.unlink(directory + '/' + file, function (err) {
                        if (err) {
                            return rej(err)
                        }
                        res();
                    });
                });
            })).then(function () {
                fs.rmdir(directory, function (err) {
                    if (err) {
                        return reject(err);
                    }
                    resolve();
                });
            }, reject);
        })
    });
}

/**
 * Prune databases, keeping the most recent `this.daysToKeep` database and
 * deleting all others.
 * @private
 */
DatabaseManager.prototype.pruneDatabases = function () {
    console.log(`LevelDB: Starting AutoPrune of databases, keeping last ${this.daysToKeep} days of data.`);
    var databases = fs.readdirSync(this.dbPath)
        .filter(function (filename) {
            return /^\d{4}-\d{2}-\d{2}$/.test(filename) &&
                fs.statSync(this.getDbPath(filename)).isDirectory();
        }, this).sort();

    var toPrune = databases.slice(0, -this.daysToKeep);

    toPrune = toPrune.filter(function (dbKey) {
        if (!this.lastUsed[dbKey]) {
            return true;
        }
        if ((Date.now() - this.lastUsed[dbKey]) < 5000) {
            console.log(`LevelDB: ${dbKey} was used in last 5 seconds, will not prune database.`);
            return false;
        }
        return true;
    }, this);

    toPrune.forEach(function (dbKey) {
        if (this.cache[dbKey]) {
            this.cache[dbKey].close();
            delete this.cache[dbKey];
            delete this.lastUsed[dbKey];
        }
    }, this);

    var pruned = 0;
    var errored = 0;

    Promise.all(toPrune.map(function (dbKey) {
        console.log(`LevelDB: Pruning ${dbKey}`);
        var path = this.getDbPath(dbKey);
        return rmr(path).then(function () {
            console.log(`LevelDB: Successfully pruned ${dbKey}`);
            pruned++;
        }, function (err) {
            console.log(`LevelDB: Error Pruning ${dbKey}, ${err}`);
            errored++;
        });
    }, this)).then(function () {
        console.log(`LevelDB: Done pruning databases, ${pruned} successful, ${errored} errored.`);
    });
};

/**
 * Close databases that haven't been used in past hour.
 * @private
 */
DatabaseManager.prototype.closeUnusedDatabases = function () {
    var closeBefore = Date.now() - 1 * 60 * 60 * 1000 // one hour;
    Object.keys(this.lastUsed).forEach(function (dbKey) {
        if (this.lastUsed[dbKey] <= closeBefore) {
            this.cache[dbKey].close();
            delete this.cache[dbKey];
            delete this.lastUsed[dbKey];
        }
    }, this);
};

/**
 * Returns an array of the "latest" database, usually just one.
 */
DatabaseManager.prototype.getLatestDatabaseKeys = function () {
    return fs.readdirSync(this.dbPath)
        .filter(function (filename) {
            return /^\d{4}-\d{2}-\d{2}$/.test(filename) &&
                fs.statSync(this.getDbPath(filename)).isDirectory();
        }, this).sort().slice(-3);
}

/**
 * Return the database key for a given date.
 *
 */
DatabaseManager.prototype.getDatabaseKey = function (date) {
    return date.toISOString().slice(0, 10);
};

/**
 * Get a database, creating it if it doesn't exist.  Takes either a date or a
 * dbKey.
 */
DatabaseManager.prototype.getDatabase = function (dbKey) {
    if (dbKey instanceof Date) {
        dbKey = this.getDatabaseKey(dbKey);
    }
    if (!this.cache[dbKey]) {
        this.cache[dbKey] = level(this.getDbPath(dbKey), {
            valueEncoding: 'binary'
        });
    }
    this.lastUsed[dbKey] = Date.now();
    return this.cache[dbKey];
};

/**
 * Given a start and end date, return a wrapper that performs actions across
 * all databases in the range and merges the results.  Does not create databases
 * if they don't exist.
 */
DatabaseManager.prototype.getMultiDBWrapper = function (start, end) {
    if (start && end) {
        var startKey = this.getDatabaseKey(start);
        var endKey = this.getDatabaseKey(end);
        var days = [start];
        while (startKey !== endKey) {
            start = new Date(start.getTime() + 24 * 60 * 60 * 1000);
            startKey = this.getDatabaseKey(start);
            days.push(start);
        }

        var databases = days.filter(function (day) {
                return this.dbExists(this.getDatabaseKey(day));
            }, this)
            .map(this.getDatabase, this);
    } else {
        var databases = this.getLatestDatabaseKeys().map(this.getDatabase, this);
    }

    function singleDBRead(database, options) {
        return new Promise(function (resolve, reject) {
            var results = [];
            database.createReadStream(options)
                .on('data', function (data) {
                    results.push(data);
                }).on('end', function () {
                    resolve(results);
                });
        });
    }

    function multiDBRead(options) {
        return Promise.all(databases.map(function (database) {
            return singleDBRead(database, options);
        })).then(function (res) {
            return res.reduce(function (a, b) {
                return a.concat(b);
            }, []);
        });
    }

    return {
        read: multiDBRead
    };
}

module.exports = DatabaseManager;
