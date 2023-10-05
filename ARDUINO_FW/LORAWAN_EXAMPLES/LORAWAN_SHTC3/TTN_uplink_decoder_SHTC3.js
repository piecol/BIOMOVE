function decodeUplink(input) {
  var data = {};

  data.temperature  = ((input.bytes[0] << 8) + input.bytes[1])/100;
  data.humidity     = ((input.bytes[2] << 8) + input.bytes[3])/100;
  data.battery      = ((input.bytes[4] << 8) + input.bytes[5])/1000;

  
  return {
    data: data,

  }
}
