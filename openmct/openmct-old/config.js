(function(exports){

  // Configure values for webapp
  
  // Required
  const target = 'ref';           // Target Name
  const nodeSite = '127.0.0.1';   // Host to serve the single page application
  const tcpSite = '127.0.0.1';    // Host of the TCP server
  const tcpPort = 50000;          // Port for the TCP server

  // Can keep as default
  const staticPort = 8080;      // Port for single page application
  const realTelemPort = 1337;   // Port streaming live telemetry datums to client
  const commandPort = 1339;     // Port to listen for commands from client

  exports.values = function() {
    return {
      target: target,
      nodeSite: nodeSite,
      staticPort: staticPort,
      tcpSite: tcpSite,
      tcpPort: tcpPort,
      realTelemPort: realTelemPort,
      commandPort: commandPort
    }
  };

}(typeof exports === 'undefined' ? this.config = {} : exports));
