import { Station } from "./models";

let radioBrowserUrl = "https://nl1.api.radio-browser.info";

const getStationsByName = async (name: string): Promise<Station[]> => {
    return fetch(`${radioBrowserUrl}/json/stations/byname/${name}`)
        .then(response => response.json())
        .catch(error => console.error(error));
}

export { getStationsByName }