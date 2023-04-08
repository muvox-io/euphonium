import { BaseConfigurationField, ConfigurationField } from "../../../api/euphonium/plugins/models";

export interface FieldProps<T extends BaseConfigurationField = BaseConfigurationField> {
    pluginName: string;
    field: T;
}
