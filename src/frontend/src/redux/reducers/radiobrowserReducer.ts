import { Action, PayloadAction, ThunkAction, createSlice, nanoid } from "@reduxjs/toolkit";
import { Station } from "../../api/radiobrowser/models";
import { RootState } from "../store";
import { radioApiEndpoints } from "../api/radiobrowser/radioApi";
import sleep from "../../utils/sleep";

export interface StationInReducer {
  url: string;
  name: string;
  bitrate: number;
  codec: string;
  countrycode: string;
  favicon: string;
  favorite?: boolean;
}

export interface RadioBrowserReducerState {
  favorites: StationInReducer[];
  results: StationInReducer[];
  searchDebounceId: string;
  isSearching: boolean;
}

const mapStationFromRadioBrowser = ({ name, favicon, url_resolved, codec, bitrate, countrycode }: Station): StationInReducer => {
  return {
    name,
    favicon,
    codec,
    bitrate,
    countrycode,
    url: url_resolved,
    favorite: false
  }
}

const applyFavoritesToResults = (favorites: StationInReducer[], results: StationInReducer[]): StationInReducer[] => results.map((station) => {
  // Check if station is in favorites
  const favorite = favorites.find((fav) => fav.url === station.url);

  return {
    ...station,
    favorite: favorite ? true : false,
  }
});

/**
 * Debounce time for radio search bar in ms.
 */
const RADIO_SEARCH_DEBOUNCE_TIME = 150;

export const searchStationsByName = (query: string): ThunkAction<void, RootState, void, Action> => {
  return async (dispatch, getState) => {
    const debounceId = nanoid();
    dispatch(setSearchDebounceId(debounceId));
    await sleep(RADIO_SEARCH_DEBOUNCE_TIME);
    if (getState().radiobrowser.searchDebounceId !== debounceId) {
      return;
    }

    if (query?.length > 0) {

      // @TODO: Add pagination
      await dispatch(
        radioApiEndpoints.getStationsByName.initiate({ name: query, limit: 30, offset: 0 })
      );
    } else {
      // Search query is empty, disable searching
      dispatch(disableSearching({}));
    }
  }
};

const radiobrowserSlice = createSlice({
  name: "radiobrowser",
  initialState: {
    favorites: [],
    results: [],
    searchDebounceId: "",
    isSearching: false,
  } as RadioBrowserReducerState,
  reducers: {
    stationResultsReceivedFromApi: (
      state,
      action: PayloadAction<Station[]>
    ) => ({
      ...state,
      isSearching: true,
      results: applyFavoritesToResults(state.favorites, action.payload.map(mapStationFromRadioBrowser))
    }),
    favoritesReceivedFromApi: (state, action: PayloadAction<StationInReducer[]>) => ({
      ...state,
      favorites: action.payload,
      results: applyFavoritesToResults(action.payload, state.results)
    }),

    setSearchDebounceId: (state, action) => {
      state.searchDebounceId = action.payload;
    },

    disableSearching: (state, action) => {
      state.isSearching = false;
    }
  },
});

export const radiobrowserReducer = radiobrowserSlice.reducer;

export const { stationResultsReceivedFromApi, favoritesReceivedFromApi, setSearchDebounceId, disableSearching } = radiobrowserSlice.actions;
