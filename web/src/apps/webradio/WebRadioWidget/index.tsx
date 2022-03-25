import IconCard from "../../../components/ui/IconCard"

const SingleRadio = ({ name = "" }) => {
    return (<div class="flex flex-row mt-4 mb-4 space-x-4">
        <img class="w-7 h-7 rounded-full mr-3" src="https://play-lh.googleusercontent.com/LEl2l7XwoWhcpHcOpxq-9zTkYPAeyeOHfcWWMBtEN-oS_LfXjQK_zv7W4Gsccmao3LE" />
        Radio 357
    </div>)
}

export default () => {
    return (
        <IconCard iconName="radio" label="Internet Radio">
            <div class="flex flex-col">
                <div class="text-app-text-secondary text-sm">Favorites</div>
                <SingleRadio />
            </div>
        </IconCard>
    )
}