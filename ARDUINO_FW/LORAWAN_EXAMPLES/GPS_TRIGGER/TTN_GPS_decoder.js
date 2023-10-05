function decodeUplink(input) {
	// Decode an uplink message from a buffer
	// (array) of bytes to an object of fields.
  var data = {};
	if (input.bytes[0] == 0x09) {
		// GPS data
		data.latitude = (input.bytes[4] | input.bytes[3] << 8 | input.bytes[2] << 16 | input.bytes[1] << 24 | (input.bytes[1] & 0x80 ? 0xFF << 24 : 0)) / 10000000.0;
		data.longitude = (input.bytes[9] | input.bytes[8] << 8 | input.bytes[7] << 16 | input.bytes[6] << 24 | (input.bytes[6] & 0x80 ? 0xFF << 24 : 0)) / 10000000.0;
		data.e_w = input.bytes[5];
		data.n_s = input.bytes[10];
	}
  return {
    data: data,

  }
}
