{
  <{string}target name> : {
    "channels" : {
      "-1": { // Always channel to stream events
        "component": null, 
        "description": "Events are shown here", 
        "format_string": null, 
        "id": -1, 
        "name": "Events", 
        "telem_type": "channel", 
        "type": "string"
      }, 
      <{string}channel id>: {
        "component": <{string}>,
        "description": <{string}>,
        "id": <{number} channel id>,
        "format_string": <{string} With only 1 python-type string modifier>,
        "limits": {
          "high_orange": <{number}>, 
          "high_red": <{number}>, 
          "high_yellow": <{number}>, 
          "low_orange": <{number}>, 
          "low_red": <{number}>, 
          "low_yellow": <{number}>
        },
        "name": <{string}>,
        "telem_type": "channel",
        "type": "Either integer-type or Enum",
        {If "type"=Enum} "enum_dict": {
          <{string} integer key>: <{string} corresponding string>,
          ...
        },
        "units": {can be null if none} [
          {
            "Gain": <{number}>
            "Label": <{string}>
            "Offset": <{number}>
          },
         ...
        ]
      },
      ...

    },
    "commands" : {
      <{string} Opcode> : {
        "arguments": [
          {
            "description": <{string}>,
            "name": <{string}>,
            "type": <{string} From type table>
          },
          ...
        ],
        "component": <{string}>,
        "description": <{string}>,
        "id": <{number}>,
        "name": <{string}>
      }
    },
    "events": {}
  }
}


