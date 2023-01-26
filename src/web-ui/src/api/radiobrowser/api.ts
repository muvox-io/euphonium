import { Station } from "./models";

let radioBrowserUrl = "https://radio-browser.gkindustries.pl";

const getStationsByName = async (name: string, limit: number, offset: number): Promise<Station[]> => {
    return fetch(`${radioBrowserUrl}/json/stations/byname/${name}?offset=${offset}&limit=${limit}&hidebroken=true`)
        .then(response => response.json())
        .catch(error => console.error(error));
}

export { getStationsByName }
