function Decoder(bytes, port) {
    
    // Basic dictionary that holds all the decoded measurement values
    var decoded = {};
    
    // Working with bytes for decoding of payload
    if (port === 2) {
        
        // Adapt this to the payload byte structure of the messages your device is sending
        //var temperature = (bytes[0] << 8 | bytes[1]) / 100.0;
        //var humidity = (bytes[2] << 8 | bytes[3]) / 100.0;
        //var battery = (bytes[4] << 8 | bytes[5])  / 1000.0;

		// GPS data
		    var lat = (bytes[4] | bytes[3] << 8 | bytes[2] << 16 | bytes[1] << 24 | (bytes[1] & 0x80 ? 0xFF << 24 : 0)) / 10000000.0;
	    	var lon = (bytes[9] | bytes[8] << 8 | bytes[7] << 16 | bytes[6] << 24 | (bytes[6] & 0x80 ? 0xFF << 24 : 0)) / 10000000.0;
	    	var e_w = bytes[5];
	    	var n_s = bytes[10];
        
        console.log("received sensor data");
        
        decoded.lat = lat;
        decoded.lon = lon;
        decoded.device_location = "(" + lat + "," + lon + ")";
        
        // Working with battery
        //decoded.battery = battery
        
        // Working with Sensor data
        //decoded.temperature1 = temperature;
        //decoded.humidity = humidity;
        
        // Working with status and booleans
        //decoded.leakage_detected = false;
        
        // Working with Configuration Fields
        var needsCalibration = configurationValues["SET_INDIVIDUAL_CALIBRATION"];
        
        if (needsCalibration) {
            var temperatureOffset = configurationValues["TEMPERATURE_CALIBRATION_FACTOR"];
            decoded.temperature = decoded.temperature + temperatureOffset
        }
        
    } else if (port === 100) {
        
        console.log("received configuration data");
    }    

    // Extract Gateway Metadata and LoRaWAN statistics
    // We are encapsulating this with a Try-Block in case the Gateway does not provide metadata such as RSSI, SNR, etc.
    try {
        decoded.lorawan_rssi = (!!normalizedPayload.gateways && !!normalizedPayload.gateways[0] && normalizedPayload.gateways[0].rssi) || 0;
        decoded.lorawan_snr = (!!normalizedPayload.gateways && !!normalizedPayload.gateways[0] && normalizedPayload.gateways[0].snr) || 0;
        decoded.lorawan_datarate = normalizedPayload.data_rate;           
    } catch (e) {
        console.log("Failed to read gateway metadata");
    }

    // Return data to datacake
    return decoded;

}
