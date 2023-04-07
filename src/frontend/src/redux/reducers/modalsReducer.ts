import { createAction, createReducer } from "@reduxjs/toolkit";

export enum ModalType {
  CONNECTION_LOST = "CONNECTION_LOST",
  FORM_MODAL = "FORM_MODAL",
}

export interface ModalConfig<D = any> {
  id: string;
  title: string;
  dismissable: boolean;
  type: ModalType;
  priority: number;
  data: D;
}

export interface ModalsState {
  modals: ModalConfig[];
}

export const initialState: ModalsState = {
  modals: [] as ModalConfig[],
};

export const pushModal = createAction<ModalConfig>("modals/pushModal");
export const popModal = createAction<string>("modals/popModalById");

export const modalsReducer = createReducer(initialState, (builder) => {
  builder
    .addCase(pushModal, (state, action) => {
      state.modals.push(action.payload);
      state.modals.sort((a, b) => a.priority - b.priority);
    })
    .addCase(popModal, (state, action) => {
      state.modals = state.modals.filter((m) => m.id !== action.payload);
    });
});
