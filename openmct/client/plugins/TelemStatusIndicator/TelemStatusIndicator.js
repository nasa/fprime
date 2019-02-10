/**
  * TelemStatusIndicator.js
  *
  * Simple telemetry status indicator which polls the F' Telemetry Server for
  * the status of the incoming data stream
  *
  * @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
  */

var TelemStatusIndicator = function (interval) {

    var pollInterval = interval || 1; //polling interval in seconds

    var statusIndicator = openmct.indicators.simpleIndicator();

    var packetRecvNode = document.createElement('span');

    var timerID;

    // CSS classes and messages for the possible response states of the server
    var statuses = {
        'data': {
            class: 's-status-ok',
            message: 'Connected to server, streaming data received.'
        },
        'noData': {
            class: 's-status-available',
            message: 'Connected to server, no streaming data.'
        },
        'noConnection': {
            class: 's-status-error',
            message: 'No connection to telemetry server.'
        }
    }

    return function install (openmct) {

        //Create simple indicator for connection status
        statusIndicator.iconClass("icon-connectivity");
        statusIndicator.statusClass(statuses.noConnection.class);
        statusIndicator.text(statuses.noConnection.message);
        statusIndicator.description(`Telemetry Server Status, polling every ${pollInterval} seconds`);
        openmct.indicators.add(statusIndicator);

        //Create "packets received" text in indicator tray
        packetRecvNode.innerText = "Packets Received: 0 |";
        openmct.indicators.add({
            element: packetRecvNode
        });

        //Start polling the realtime server's status endpoint
        poll();
    }

    function poll() {
        timerID = setTimeout(function () {
            http.get('/realtime/status').then(function (result) {
                status = result.data.status;
                packetCount = result.data.packetCount;
                statusIndicator.statusClass(statuses[status].class);
                statusIndicator.text(statuses[status].message);
                packetRecvNode.innerText = `Packets received: ${packetCount} |`
            }).catch(function (err) {
                // Stop polling when server disconnects, to prevent a misleading "connected"
                // status indicator if server is restarted without a browser refresh
                statusIndicator.statusClass(statuses.noConnection.class);
                statusIndicator.text(statuses.noConnection.message);
                clearTimeout(timerID);
            });
            poll();
        }, pollInterval * 1000);
    }
}
