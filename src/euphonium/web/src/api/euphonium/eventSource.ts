import getBaseUrl from "./baseUrl";

export default new EventSource(getBaseUrl() + "/events");
