import { useEffect } from "preact/hooks";
import React from "react";
import { useDispatch } from "react-redux";
import {
  ModalConfig,
  ModalType,
  popModal,
  pushModal,
} from "../../redux/reducers/modalsReducer";

const ModalTrigger = (
  props: Partial<ModalConfig> & {
    type: ModalType;
    id: string;
  }
) => {
  const dispatch = useDispatch();
  useEffect(() => {
    const propsWithDefaults = {
      dismissable: false,
      priority: 0,
      title: "",
      data: null,
      ...props,
    };
    dispatch(pushModal(propsWithDefaults));
    return () => {
      dispatch(popModal(propsWithDefaults.id));
    };
  }, []);
  return null;
};

export default ModalTrigger;
