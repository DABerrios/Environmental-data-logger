/**
 * Payload formatter for The Things Stack.
 * Decodes the given uplink input bytes into a readable JSON format.
 *
 * @param {Object} input - The uplink input object.
 * @param {Uint8Array} input.bytes - The payload bytes to decode.
 * @returns {Object} The decoded data in JSON format.
 * @returns {Object} return.data - The decoded sensor data.
 * @returns {string} return.data.rain - Rainfall in mm.
 * @returns {string} return.data.temperature - Temperature in °C.
 * @returns {string} return.data.humidity - Humidity in %.
 * @returns {string} return.data.pressure - Atmospheric pressure in hPa.
 * @returns {string} return.data.date - Date in DD/MM/YYYY format.
 * @returns {string} return.data.time - Time in HH:MM:SS format.
 * @returns {number} return.data.id - Sensor ID.
 * @returns {Object} [return.errors] - Errors encountered during decoding.
 */
function decodeUplink(input) {
    const bytes = input.bytes;
    if (bytes.length < 13) {
      return {
        errors: ["Payload size too small"],
      };
    }
  
    let index = 0;
  
    // Decode Rain (2 bytes)
    const rainInt = (bytes[index] << 8) | bytes[index + 1];
    const rain = rainInt / 100.0; // Convert to float (mm)
    index += 2;
  
    // Decode Temperature (2 bytes)
    const tempInt = (bytes[index] << 8) | bytes[index + 1];
    const temperature = tempInt / 100.0; // Convert to float (°C)
    index += 2;
  
    // Decode Humidity (2 bytes)
    const humInt = (bytes[index] << 8) | bytes[index + 1];
    const humidity = humInt / 100.0; // Convert to float (%)
    index += 2;
  
    // Decode Pressure (3 bytes)
    const pressInt = (bytes[index] << 16) | (bytes[index + 1] << 8) | bytes[index + 2];
    const pressure = pressInt / 100.0; // Convert to float (hPa)
    index += 3;
  
    // Decode Date (2 bytes)
    const dateInt = (bytes[index] << 8) | bytes[index + 1];
    const year = 2000 + ((dateInt >> 9) & 0x7F); // Extract year
    const month = (dateInt >> 5) & 0x0F; // Extract month
    const day = dateInt & 0x1F; // Extract day
    index += 2;
  
    // Decode Time (2 bytes)
    const timeInt = (bytes[index] << 8) | bytes[index + 1];
    const hour = (timeInt >> 11) & 0x1F; // Extract hour
    const minute = (timeInt >> 5) & 0x3F; // Extract minute
    const second = (timeInt & 0x1F) * 2; // Extract second
    index += 2;
  
    // Decode ID (1 byte)
    const id = bytes[index];
  
    // Return the decoded data in JSON format
    return {
      data: {
        rain: rain.toFixed(2), // in mm
        temperature: temperature.toFixed(2), // in °C
        humidity: humidity.toFixed(2), // in %
        pressure: pressure.toFixed(2), // in hPa
        date: `${String(day).padStart(2, "0")}/${String(month).padStart(2, "0")}/${year}`, // formatted date
        time: `${String(hour).padStart(2, "0")}:${String(minute).padStart(2, "0")}:${String(second).padStart(2, "0")}`, // formatted time
        id: id,
      },
    };
  }