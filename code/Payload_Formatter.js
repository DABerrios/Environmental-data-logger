
/**
 * Decodes the uplink payload from a byte array.
 *
 * @param {Object} input - The input object containing the byte array.
 * @param {Uint8Array} input.bytes - The byte array to decode.
 * @returns {Object} The decoded data or an error message if the payload size is too small.
 * @returns {Object} return.data - The decoded data.
 * @returns {string} return.data.rain - The decoded rain value in mm.
 * @returns {string} return.data.temperature - The decoded temperature value in °C.
 * @returns {string} return.data.humidity - The decoded humidity value in %.
 * @returns {string} return.data.pressure - The decoded pressure value in hPa.
 * @returns {number} return.data.timestamp - The Unix timestamp.
 * @returns {number} return.data.id - The decoded ID.
 * @returns {Object} return.errors - The error message if the payload size is too small.
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

  // Convert date and time to Unix timestamp
  const timestamp = Math.floor(
    new Date(year, month - 1, day, hour, minute, second).getTime() / 1000
  );

  // Return the decoded data in JSON format
  return {
    data: {
      rain: rain.toFixed(2), // in mm
      temperature: temperature.toFixed(2), // in °C
      humidity: humidity.toFixed(2), // in %
      pressure: pressure.toFixed(2), // in hPa
      timestamp: timestamp, // Unix timestamp (numeric)
      id: id,
    },
  };
}