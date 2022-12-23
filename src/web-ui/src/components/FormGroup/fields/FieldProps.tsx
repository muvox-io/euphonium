import { ConfigurationField } from "../../../api/euphonium/plugins/models";

export interface FieldProps<T> {
    field: ConfigurationField;
    value: T;
    onChange: (value: T) => void;

    /**
     * Called to reload other field states.
     */
    onChangeFinished?: () => void;
}
