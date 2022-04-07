import { ConfigurationField } from "../../../api/euphonium/plugins/models";

export interface FieldProps<T> {
    field: ConfigurationField;
    value: T;
    onChange: (value: T) => void;
}
