Here is a basic `README.md` file for your project, suitable for Docker Hub:

---

# Unix Geo Server

This project is a Unix domain socket server that provides country and flag information for domain names by performing a DNS lookup. The server uses the MaxMind GeoLite2 database to resolve IP addresses to country information.

## Features

- **Unix Domain Socket Server:** Handles incoming requests for domain-to-country lookups.
- **GeoIP Lookup:** Uses the MaxMind GeoLite2 database to determine the country from an IP address.
- **JSON Output:** Returns country and flag information in a JSON format.
- **Cross-Origin Resource Sharing (CORS):** Allows requests from any origin.
- **DNS Lookup:** Retrieves IP addresses using the `dig` command for a given domain.

## Usage

### Docker

To use this server via Docker, you can build and run the container using the provided `Dockerfile`.

### Building the Docker Image

Clone the repository and navigate to the project directory, then build the image:

```bash
docker build -t unix-geo-server .
```

### Running the Container

To run the container:

```bash
docker run -d --name unix-geo-server unix-geo-server
```

This will start the server and listen for connections on `/tmp/myserver.sock`.

### Accessing the Server

The server listens for requests over a Unix domain socket located at `/tmp/myserver.sock`. Clients can connect and send HTTP requests to query the country information for a domain.

#### Example Request

The server expects a request like:

```
GET /what-is-country/example.com HTTP/1.1
```

#### Example Response

The server returns a JSON response with the IP addresses and country information for the requested domain:

```json
{
  "ips": "93.184.216.34 ",
  "flagImg": "iVBORw0KGgoAAAANSUhEUgAAABAAAAAMCAMAAABcOc2zAAABHVBMVEWMGSlPT3daWn9dXICMHCyNIi54ER2RHy54ERxXVnpQT3Z5M0+aJzqIGSmQIjBuDRaQIS2QIS5+FSB8Ex5zDxpnZpBjYozFWGXx2Nvcm6PDUF+7Q1HgrLLKbnnz3uHu09bUjpbk0dNmZY5vbpXmub+/TFrgqbCzPUqoWGPKmaBXVntqaZJfXolWVYPdoqrViJLp2d7ir7bFeIKvg4m+VGHBXGfXur6fSFPmxMjjtbqYSVPHrrGqeoCxanO0RFBfXoJQT3WZYHral5+fb4lNS3GpjKPm3uXg3eWWTmq5ucyqnLDAX2qJN0CznJ90dI7KY3HUk5rsyc7epKvTgIuYZmuvXmmNT1WgTVfUhI+wdXy5i5K+pqnAkpfAZG7RiZFR4mcfAAAAFXRSTlMgOpHITDGTct3b4bmVPss6PstvucjjrrfjAAAAxklEQVQIHQXBBUICQRQA0A8CuyghyMzsbHeQS0h3qZRBl/c/Bu9BIPgUfY6+xJhQGOCVScEA64aBF3WvsnfdnneFrJFDGH3y31tK6Ym0IYsQ0nMzRf4t27Ypd2Go5zHCE8kSeEIEqwtThPOFwmhsKpwsc2IbNl8fvj/XxJJUc4SluoYq4QnhBUtSi63yz3IFF84sUrHJKS36r9pKFRqk9CeqlXenX5PufWcHHa93bLjnW13zm5rWOUCCiQOEQ+nIG8uymUjyAUFxHXDIwv3pAAAAAElFTkSuQmCC",
  "countryName": "United States"
}
```

## Required Files

### Source Files

- **`unix-server.c`** - The main server implementation.
- **`geo_lookup.c`** - Handles GeoIP lookup using MaxMind.
- **`geo_lookup.h`** - Header file for the GeoIP lookup functions.

### Dependencies

- **libjson-c:** For handling JSON responses.
- **libmaxminddb:** For GeoIP lookup from the MaxMind database.

### Additional Requirements

- **GeoLite2-City.mmdb**: You need to download the [MaxMind GeoLite2](https://dev.maxmind.com/geoip/geolite2-free-geolocation-data) database and place it in the same directory as the server.

## Development

To compile and run the server locally (without Docker), ensure you have the necessary libraries installed:

```bash
gcc unix-server.c geo_lookup.c -o unix-geo-server -ljson-c -lmaxminddb
```

Run the server:

```bash
./unix-geo-server
```

## License

This project is licensed under the MIT License.

---

Let me know if you need to add or adjust any parts!