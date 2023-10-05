function Decoder(bytes, port) {
    
     // Output normalized Payload
    //console.log(JSON.stringify(normalizedPayload,0,4));
    
    // Output raw payload coming from webhook of your LNS
    //console.log(JSON.stringify(rawPayload,0,4));    
    
    
    // Basic dictionary that holds all the decoded measurement values
    var decoded = {};
    
    // Working with bytes for decoding of payload
    if (port === 2) {
        
        // Adapt this to the payload byte structure of the messages your device is sending
        var temperature = (bytes[0] << 8 | bytes[1]) / 100.0;
        var humidity = (bytes[2] << 8 | bytes[3]) / 100.0;
        var battery = (bytes[4] << 8 | bytes[5])  / 1000.0;
        
        console.log("received sensor data");
        

        
        // Working with battery
        decoded.battery = battery
        
        // Working with Sensor data
        decoded.temperature = temperature;
        decoded.humidity = humidity;
        
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
