import IconCard from "../../../components/ui/IconCard"

const SingleRadio = ({ name = "" }) => {
    return (<div class="flex flex-row mt-1 mb-1 space-x-4 items-center">
        <img class="w-5 h-5 rounded-full mr-2" src="https://play-lh.googleusercontent.com/LEl2l7XwoWhcpHcOpxq-9zTkYPAeyeOHfcWWMBtEN-oS_LfXjQK_zv7W4Gsccmao3LE" />
        Radio 357
    </div>)
}

export default () => {
    return (
        <IconCard iconName="radio" label="Internet Radio">
            <div class="flex flex-col">
                <div class="text-app-text-secondary text-sm mb-1">Favorites</div>
                {/* <SingleRadio />
                <SingleRadio />
                <SingleRadio /> */}
                <div class="text-app-text-secondary text-sm mb-1 mt-2">Recently played</div>
                {/* <SingleRadio />
                <SingleRadio />
                <SingleRadio /> */}
            </div>
        </IconCard>
    )
}