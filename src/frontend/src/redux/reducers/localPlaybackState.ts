import {
  Action,
  AsyncThunk,
  PayloadAction,
  ThunkAction,
  createSlice,
  nanoid,
} from "@reduxjs/toolkit";
import { PlaybackState } from "../../api/euphonium/playback/models";
import { RootState } from "../store";
import sleep from "../../utils/sleep";
import { playbackApiEndpoints } from "../api/euphonium/playbackApi";

export interface LocalPlaybackState extends PlaybackState {
  localVolume: number;
  volumeDebounceId: string;
}

/**
 * Time to update the volume on the server.
 */
const VOLUME_UPDATE_DEBOUNCE_TIME = 150;

/**
 * Time to update the volume in persistent storage.
 * (This shold be kept long so that the flash in the device does not wear out from spamming writes)
 */
const PERSIST_VOLUME_DEBOUNCE_TIME = 1500;

/**
 * Should be dispatched when the volume is changed through the UI.
 * The localVolume will be updated immediately, but the volume update will
 * be debounced.
 */
export const onVolumeChange = (
  volume: number
): ThunkAction<void, RootState, void, Action> => {
  if (typeof volume !== "number") {
    throw new Error("Volume must be a number");
  }
  return async (dispatch, getState) => {
    dispatch(setLocalVolume(volume));
    const debounceId = nanoid();
    dispatch(setVolumeDebounceId(debounceId));
    await sleep(VOLUME_UPDATE_DEBOUNCE_TIME);
    if (getState().localPlaybackState.volumeDebounceId !== debounceId) {
      return;
    }
    await dispatch(
      playbackApiEndpoints.updateVolume.initiate({ volume, persist: false })
    );
    await sleep(PERSIST_VOLUME_DEBOUNCE_TIME);
    if (getState().localPlaybackState.volumeDebounceId !== debounceId) {
      return;
    }
    await dispatch(
      playbackApiEndpoints.updateVolume.initiate({ volume, persist: true })
    );
  };
};

const slice = createSlice({
  name: "localPlaybackState",
  initialState: {
    localVolume: 0,
    volumeDebounceId: "",
  } as LocalPlaybackState,
  reducers: {
    playbackStateReceivedFromServer: (
      state,
      action: PayloadAction<PlaybackState>
    ) => {
      return {
        ...state,
        ...action.payload,
        localVolume: action.payload.settings.volume,
      };
    },
    setLocalVolume: (state, action) => {
      state.localVolume = action.payload;
    },
    setVolumeDebounceId: (state, action) => {
      state.volumeDebounceId = action.payload;
    },
  },
});

export const {
  setLocalVolume,
  setVolumeDebounceId,
  playbackStateReceivedFromServer,
} = slice.actions;
export const localPlaybackStateReducer = slice.reducer;
