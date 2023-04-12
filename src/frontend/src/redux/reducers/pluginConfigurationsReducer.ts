import {
  Action,
  createAction,
  createAsyncThunk,
  createReducer,
  ThunkAction,
} from "@reduxjs/toolkit";
import { pluginsApiEndpoints } from "../api/euphonium/pluginsApi";
import { PluginInterfaceEvent } from "../api/euphonium/pluginsApiModel";
import { RootState } from "../store";

export interface PluginConfigurationsState {
  states: {
    [pluginName: string]: {
      state: any;
      dirty: boolean;
      scheduledSendStateRequestId: number; // Used to debounce sending state to server
    };
  };
}

const initialState: PluginConfigurationsState = {
  states: {},
};

/**
 * Whole plugin state is received from the server.
 */
export const stateFromServerApplied = createAction<{
  pluginName: string;
  state: any;
}>("pluginConfigurations/stateFromServerApplied");

/**
 * A single field has been modified client-side.
 */
export const stateKeyChanged = createAction<{
  pluginName: string;
  key: string;
  value: any;
  setScheduledSendStateRequestId?: number;
}>("pluginConfigurations/stateKeyChanged");

export const resetScheduledSendStateRequestId = createAction<{
  pluginName: string;
}>("pluginConfigurations/resetScheduledSendStateRequestId");

export interface OnFieldChangedPayload {
  pluginName: string;
  key: string;
  value: any;
  debounce: boolean;
}

export interface OnFieldEvent {
  pluginName: string;
  event: PluginInterfaceEvent;
}

const DEBOUNCE_TIME = 500;

export const onFieldChanged =
  (
    payload: OnFieldChangedPayload
  ): ThunkAction<void, RootState, void, Action> =>
  async (dispatch, getState) => {
    const { pluginName, key, value, debounce } = payload;
    const myDebounceId = Math.floor(Math.random() * 1000000) + 1;
    dispatch(
      stateKeyChanged({
        pluginName,
        key,
        value,
        setScheduledSendStateRequestId: myDebounceId,
      })
    );
    if (payload.debounce) {
      await new Promise((resolve) => setTimeout(resolve, DEBOUNCE_TIME));
    }
    if (
      (getState() as RootState).pluginConfigurations.states[pluginName]
        .scheduledSendStateRequestId !== myDebounceId
    ) {
      // a newer request has been scheduled, so we don't need to send this one
      return;
    }
    await dispatch(
      pluginsApiEndpoints.postPluginConfiguration.initiate({
        pluginName,
        state: (getState() as RootState).pluginConfigurations.states[pluginName]
          .state,
        isPreview: true,
        events: [],
      })
    );
  };

export const onFieldEvent =
  (payload: {
    pluginName: string;
    event: PluginInterfaceEvent;
  }): ThunkAction<void, RootState, void, Action> =>
  async (dispatch, getState) => {
    // stop any debounced state updates
    dispatch(
      resetScheduledSendStateRequestId({ pluginName: payload.pluginName })
    );

    await dispatch(
      pluginsApiEndpoints.postPluginConfiguration.initiate({
        pluginName: payload.pluginName,
        state: (getState() as RootState).pluginConfigurations.states[
          payload.pluginName
        ].state,
        isPreview: true,
        events: [payload.event],
      })
    );
  };

export const pluginStateSelector =
  (pluginName: string) => (state: RootState) => {
    const pluginState = state.pluginConfigurations.states[pluginName];
    return pluginState.state;
  };

export const pluginDirtySelector =
  (pluginName: string) => (state: RootState) => {
    const pluginState = state.pluginConfigurations.states[pluginName];
    return pluginState.dirty;
  };

export const pluginStateReadySelector =
  (pluginName: string) => (state: RootState) => {
    const pluginState = state.pluginConfigurations.states[pluginName];
    return pluginState != null;
  };

export const pluginConfigurationsReducer = createReducer(
  initialState,
  (builder) => {
    builder
      .addCase(stateFromServerApplied, (state, action) => {
        const { pluginName, state: newState } = action.payload;
        state.states[pluginName] = {
          state: newState,
          dirty: false,
          scheduledSendStateRequestId: 0,
        };
      })
      .addCase(stateKeyChanged, (state, action) => {
        const { pluginName, key, value } = action.payload;
        const pluginState = state.states[pluginName];
        if (!pluginState) {
          throw new Error(`Plugin ${pluginName} not found`);
        }
        pluginState.state[key] = value;
        pluginState.dirty = true;
        if (action.payload.setScheduledSendStateRequestId != null) {
          pluginState.scheduledSendStateRequestId =
            action.payload.setScheduledSendStateRequestId;
        }
      })
      .addCase(resetScheduledSendStateRequestId, (state, action) => {
        const { pluginName } = action.payload;
        const pluginState = state.states[pluginName];
        if (!pluginState) {
          throw new Error(`Plugin ${pluginName} not found`);
        }
        pluginState.scheduledSendStateRequestId = 0;
      });
  }
);
