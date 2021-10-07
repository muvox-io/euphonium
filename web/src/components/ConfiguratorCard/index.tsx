import css from './ConfiguratorCard.module.scss'
import Input from '../Input'
import Button from '../Button'

export default () => {
    return (<div className={css.confWrapper}>
        <div className={css.confWrapper__header}>
            Spotify (cspot)
        </div>
        <div className={css.confWrapper__subheader}>
            plugin configuration
        </div>
        <div className={css.confWrapper__card}>
            <Input tooltip="Speaker's name" value="Main room"/>
            <Input tooltip="Audio quality" value="Highest (320 kbps)"/>
        </div>
        <div className={css.confWrapper__actionButton}>
            <Button/>
        </div>
    </div>)
}