//TODO : these variables and functions are currently placed into the global namespace,
// making it hard to tell where some of this functionality comes from
// Should refactor this code with more logical Vue classes and setup AMD module
// import/export
var command = {
  props: ['showResults', 'socket', 'command'],
  // Displayed HTML
  template: `
    <div class="command-search">
      <input type="text"
             class="cmd-input"
             v-model="commandQuery"
             @keyup="navigateResults"
             placeholder="<command name>: <arg>, <arg>, ..."
             ref="input"
             @click="nav.resIndex = -1">
      <ul class="cmd-results"
          v-show="showResults">
        <li v-for="command in results"
            @click="select(command)">
          <p>{{ command["name"] + ": " + command["argString"]}}</p>
        </li>
      </ul>
    </div>
  `,
  // Dependent data initiallized
  data: function () {
    return {
      commandQuery: '',
      results: [],
      nav: {
        resIndex: -1,
      },
      freezeResults: false,
      warning: ''
    }
  },
  // If any vals change do this...
  watch: {
    'commandQuery': function(val) {
      this.searchCommand(val);
    },
    'showResults': function() {
      this.searchCommand(this.commandQuery);
    },
    'warning': function(val) {
      // dev
      alert(val);
    },
    'command': function(val) {
      if (val != '') {
        this.commandQuery = val;
        this.$refs.input.focus();
      }
    }
  },
  // All the internal widget methods
  methods: {
    getCommands: function () {
      // Returns promise of array of all commands
      return getDictionary().then(function (dict) {
        let commandDict = dict['ref']['commands'];
        let validCommands = [];
        for (id in commandDict) {
          validCommands.push(commandDict[id]);
        }
        return validCommands;
      });
    },
    select: function (cmd) {
      this.commandQuery = cmd["name"] + ': ';
      this.freezeResults = true;
      this.$refs.input.focus();
    },
    searchCommand: function(query) {
      let self = this;  // Avoid 'this' scoping issues inside .then of promise
      self.getCommands().then(function (vc) {
        self.results = vc.filter(function (c) {
          let colonIndex = query.indexOf(':');
          if (colonIndex != -1) {
            return c['name'].toLowerCase() === query.toLowerCase().split(':')[0];
          } else {
            return c['name'].toLowerCase().indexOf(query.toLowerCase()) != -1;
          }
        });
        self.results.forEach(function (c) {
          if (c['arguments'].length != 0) {
            c['argString'] = c['arguments'].map( (a) => a['name'] + ' (' + a['type'] + ')');
            c['argString'] = c['argString'].join(', ');
          } else {
            c['argString'] = '<no arguments>';
          }
        });
      })
    },
    cleanCommand: function(cmd) {
      cmd.split(',').filter((char) => char != '').join(','); // Remove extra commas and whitespace
    },
    parseCmd: function(cmd) {
      if (this.results.length != 1) {
        this.warning = 'Invalid command name!';
        return false;
      }
      if (cmd.indexOf(':') == -1) {
        this.warning = 'Please add colon ( : ) to the end of the command name!';
        return false;
      }

      // Comma Deliminted arguments. Does not account for commas in a string argument

      let checkColon = cmd.split(':');
      if (checkColon.length > 2) {
        this.warning = 'Too many colons!';
        return false;
      }

      let argString = checkColon.pop();

      let argsInput = argString.split(',').filter((a) => a != '' && a != ' ');

      let commandReq = this.results[0]; // Get command info to check arguments with

      if (commandReq['arguments'].length < argsInput.length) {
        this.warning = 'Too many args!';
        return false;
      } else if (commandReq['arguments'].length > argsInput.length) {
        this.warning = 'Not enough Args!'
        return false;
      }

      userArgs = [];
      commandReq['arguments'].forEach(function (aR, i) {
        let typeReq = aR['type'];

        let userA;
        if (typeReq != 'String') {
          userA = parseInt(argsInput[i]);
          if (userA == NaN) {
            this.warning = 'Expected a number for argument ' + i.toString();
            return false;
          }
        } else {
          // If user argument should be string
          userA = argsInput[i];
        }
        // DEV More checks
        userArgs.push(userA);
      });

      // Command object that is actually sent to node.js
      return {
        'id': commandReq['id'],
        'arguments': userArgs,
        'name': commandReq['name'],
        'timestamp': Date()
      };
    },
    sendCommand: function(cmd) {
      commandToSend = this.parseCmd(cmd);
      if (commandToSend) {

        this.socket.send(JSON.stringify(commandToSend));
        // alert(JSON.stringify(commandToSend)); // Dev
      }
    },
    // Handles the 'Enter' and 'Escape' but others not yet done.
    navigateResults: function (event) {
      // Always search
      this.freezeResults = false;
      let keyPressed = event.key;
      switch (keyPressed) {
        case 'Escape': {
          this.showResults = false;
          this.nav.resIndex = -1;
          break;
        }
        case 'Enter': {
          this.sendCommand(this.commandQuery);
          break;
        }
        case 'ArrowDown': {
          let curIndex = this.nav.resIndex;
          this.nav.resIndex += ((curIndex < this.results.length) ? 1 : 0);
          break;
        }
        case 'ArrowUp': {
          let curIndex = this.nav.resIndex;
          this.nav.resIndex -= ((curIndex > -1) ? 1 : 0);
          break;
        }

        default: {
          // Nothing
          break;
        }
      }
    }
  }
}

var historySocket;
var hist = {
  props: ['socket'],
  // HTML of what you see
  template: `
    <div class="command-hist">
      <ul class="hist-results">
        <li v-for="command in commandHistory"
            @click="selectHist(command)">
          <p>{{ formatHistString(command) }}</p>
        </li>
      </ul>

      <input
        class="hist-input"
        type="text"
        v-model="historyQuery"
        placeholder="Search history">
    </div>
  `,
  // Data variables used
  data: function () {
    return {
      commandResults: [],
      commandHistory: [],
      historyQuery: ''
    }
  },
  // On change execute this function
  watch: {
    historyQuery: function (query) {
      this.searchHist(query);
    }

  },
  // Before page rendering but after instanciation.
  beforeMount() {
    this.populateWithHist();
  },
  // Continuously executed - node server to client so we see all commands
  mounted() {
    let self = this;
    self.socket.onmessage = function (event) {
      self.commandHistory.unshift(JSON.parse(event.data));
    }
  },
  // Executable methods within the widget.
  methods: {
    populateWithHist: function () {
      // Load command history from server log
      let self = this;
      self.getCommandHistory().then(function (commandHist) {
        self.commandHistory = commandHist['commands'];
      });
    },
    getCommandHistory: function () {
      // Needs directory from root of application
      return http.get('/server/logs/command-log.json').then(function (result) {
        return result.data;
      });
    },
    getCommands: function () {
      // Returns promise of array of all commands
      return getDictionary().then(function (dict) {
        let commandDict = dict['ref']['commands'];
        let validCommands = [];
        for (id in commandDict) {
          validCommands.push(commandDict[id]);
        }
        return validCommands;
      });
    },
    formatHistString: function (histObj) {
      return histObj['name'] + ': ' + histObj['arguments'].join(', ');
    },
    searchHist: function (query) {
      if (query == '') {
        this.populateWithHist();
      } else {
        let self = this;
        self.getCommandHistory().then(function (hist) {
          self.commandHistory = hist['commands'].filter(function (h) {
            return self.formatHistString(h).toLowerCase().indexOf(query.toLowerCase()) != -1;
          });
        });
      }
    },
    selectHist: function (cmd) {
      this.$emit('historySelected', this.formatHistString(cmd));
    }
  }
};

var CommandView = Vue.extend({
  // Div to attach Vue object to.
  template: $('#commandTemplate').text(),
  // Sub-objects to Vue objet
  components: {
    'command': command,
    'history': hist
  },
  // Utility methods for Vue
  methods: {
    toggleSearch: function (show) {
      this.showCmdSearchResults = show;
    }
  }
})
