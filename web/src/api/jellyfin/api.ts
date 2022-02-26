import { Station } from "./models";
import { apiUrl } from "../euphonium/api";

let jellyfinUrl = "https://jellyfin.cf";
let token = "efc4c854b9384455b852dc44ce43e6f7";
let userId = "d6081b9d808b451ca13256211a30d30d";

const getTracksByName = async (name: string): Promise<Station[]> => {
    return fetch(`${jellyfinUrl}/Items?UserId=${userId}&IncludeItemTypes=Track&Recursive=true&searchTerm=${name}`, {
    method: "GET",
    headers: {
      "x-mediabrowser-token": token,
    },
  })
    .then(response => response.json())
    .then(data => data.Items)
    .then(data => { 
        return data.filter((item: any) => item.IsFolder === false);
    })
    .catch(error => console.error(error));
}

const playTrackID = async (
  TrackID: string
): Promise<any> => {
  return await fetch(apiUrl + "/jellyfin/playByTrackID", {
    method: "POST",
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ TrackID }),
  }).then((e) => e.json());
};

export { getTracksByName }
export { playTrackID }
