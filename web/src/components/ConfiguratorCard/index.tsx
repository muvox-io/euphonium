import { useState, useEffect } from "preact/hooks";

import css from './ConfiguratorCard.module.scss'
import Input from '../Input'
import Select from '../Select'
import Button from '../Button'
import { ConfigurationField, ConfigurationFieldType } from "../../api/models";
import { getPluginConfiguration } from "../../api/api";

const renderConfigurationField = ({value, tooltip, type, listValues}: ConfigurationField) => {
    switch(type) {
        case ConfigurationFieldType.String:
            return (<Input tooltip={tooltip} value={value}/>)
        case ConfigurationFieldType.StringList:
            return (<Select tooltip={tooltip} value={value} values={listValues}/>)
        default:
            return (<p>Unsupported field type</p>)
    }
}

export default () => {
    const [configurationFields, setConfigurationFields] = useState<ConfigurationField[]>([]);
    
    useEffect(() => {
        getPluginConfiguration('cspot').then((e) => setConfigurationFields(e));
    }, []);

    return (<div className={css.confWrapper}>
        <div className={css.confWrapper__header}>
            Spotify (cspot)
        </div>
        <div className={css.confWrapper__subheader}>
            plugin configuration
        </div>
        <div className={css.confWrapper__card}>
        {configurationFields.map((field) => renderConfigurationField(field))}
        </div>
        <div className={css.confWrapper__actionButton}>
            <Button/>
        </div>
    </div>)
}