import { Station } from "./models";

let radioBrowserUrl = "https://radio-browser.gkindustries.pl";

const mapStationFromRadioBrowser = ({name, favicon, url_resolved, codec, bitrate, countrycode}: any): Station => {
  return {
    name,
    favicon,
    codec,
    bitrate,
    countrycode,
    url: url_resolved
  }
}

const getStationsByName = async (
  name: string,
  limit: number,
  offset: number
): Promise<Station[]> => {
  return fetch(
    `${radioBrowserUrl}/json/stations/byname/${name}?offset=${offset}&limit=${limit}&hidebroken=true`
  )
    .then((response) => response.json())
    .then((data) => data.map(mapStationFromRadioBrowser))
    .catch((error) => console.error(error));
};

const getTopStations = async (
  limit: number,
  offset: number
): Promise<Station[]> => {
  return fetch(
    `${radioBrowserUrl}/json/stations?offset=${offset}&limit=${limit}`
  )
    .then((response) => response.json())
    .then((data) => data.map(mapStationFromRadioBrowser))
    .catch((error) => console.error(error));
};

export { getStationsByName, getTopStations };
