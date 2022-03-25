import {
    eventSource,
    setPaused,
} from "../../api/euphonium/api";
import Icon from "../ui/Icon";
import {useEffect, useState} from "preact/hooks";
import useIsMobile from "../../utils/isMobile.hook";
import Equalizer from "../Equalizer";
import Modal from "../Modal";
import { getPlaybackState, PlaybackState, PlaybackStatus, updateVolume } from "../../api/euphonium/playback";

export function debounce<T extends unknown[], U>(
    callback: (...args: T) => PromiseLike<U> | U,
    wait: number
) {
    let timer: number;

    return (...args: T): Promise<U> => {
        clearTimeout(timer);
        return new Promise((resolve) => {
            timer = setTimeout(() => resolve(callback(...args)), wait);
        });
    };
}

const PlaybackBar = ({themeColor = "#fff"}) => {
    const [playbackState, setPlaybackState] = useState<PlaybackState>();

    useEffect(() => {
        getPlaybackState().then((e) => setPlaybackState(e));
    }, []);

    useEffect(() => {
        eventSource.addEventListener("playback", ({data}: any) => {
            setPlaybackState(JSON.parse(data));
        });
    }, []);

    const [eqOpen, setEqOpen] = useState<boolean>(false);
    const [mobileDialogOpen, setMobileDialogOpen] = useState<boolean>(false);

    const volUpdatedInstant = (volume: number, persist: boolean) => {
        updateVolume(Math.round((volume / 15) * 100), persist);
    };

    const volUpdated = debounce(
        (volume: number, persist: boolean = false) => volUpdatedInstant(volume, persist),
        100
    );

    const isMobile = useIsMobile();

    return (
        <div class="flex flex-col flex-grow bottom-0 fixed right-0 left-0 md:left-[220px]" onClick={() => {
            if (isMobile) {
                setMobileDialogOpen(true);
            }
        }}>
            <div
                class="flex flex-row bg-app-secondary border border-app-border md:bg-app-primary h-13 mr-3 ml-3 rounded-t-xl items-center shadow-xl flex-grow">
                <img
                    class="rounded-xl h-20 w-20 shadow-xl border border-app-border bg-white -mt-8 ml-2 mb-2"
                    src={
                        playbackState?.song.icon ??
                        "https://www.veryicon.com/download/png/media/music-series/sound-wave-1-2?s=256"
                    }
                ></img>
                <div class="flex flex-col justify-center ml-4 w-full">
                    <div>{playbackState?.song.songName}</div>
                    <div class="text-app-text-secondary text-xs">
                        {playbackState?.song.artistName} • {playbackState?.song.albumName} •
                        playback from {playbackState?.song.sourceName}
                    </div>
                </div>
                <div class="md:hidden mx-3 rounded-full cursor-pointer flex items-center justify-center"><Icon name="eq"/></div>
                {!isMobile ? (
                    <div class="mr-3 text-xl text-app-text-secondary ml-auto flex flex-row">
                        <div class="relative flex justify-center">
                            {eqOpen && playbackState?.eq ? (
                                <div
                                    class="absolute bottom-[50px] shadow-xl text-center rounded-xl w-[170px] p-4 z-index-2 bg-app-primary mb-1">
                                    equalizer
                                    <Equalizer eq={playbackState!!.eq}/>
                                </div>
                            ) : null}
                            <Icon onClick={() => setEqOpen(!eqOpen)} name="equalizer"/>
                        </div>
                        <Icon name="volume-up"/>
                        <input
                            class="w-20"
                            type="range"
                            id="volume"
                            name="volume"
                            value={((playbackState?.volume || 0) / 100) * 15}
                            onInput={(e: any) => volUpdated(e.target.value)}
                            onBlur={(e: any) => volUpdated(e.target.value, true)}
                            min="0"
                            max="15"
                        />
                        <Icon
                            onClick={() => {
                                setPaused(playbackState?.status == PlaybackStatus.Playing);
                            }}
                            name={
                                playbackState?.status == PlaybackStatus.Playing
                                    ? "pause"
                                    : "play"
                            }
                        />
                    </div>
                ) : null}
            </div>
            <div class="mr-3 ml-3 h-1 w-auto text-xs"
                 style={{backgroundColor: playbackState?.song?.sourceThemeColor}}></div>
            {mobileDialogOpen == true ? (<Modal header="Playback settings">
                <div class="flex flex-col">
                    <div className="text-app-text-secondary text-lg mb-3">equalizer</div>
                    <div class="w-full flex justify-center pl-[35px] pt-2 pb-4 rounded-2xl bg-app-secondary">
                        <div
                            className="text-center w-[170px]">
                            {playbackState?.eq ? (<Equalizer eq={playbackState?.eq}/>) : null}
                        </div>
                    </div>
                    <div class="text-app-text-secondary text-lg mt-4">volume setting</div>
                    <input
                        className="w-20"
                        type="range"
                        id="volume"
                        name="volume"
                        class="w-[240px] mt-2 mb-2"
                        value={((playbackState?.volume || 0) / 100) * 15}
                        onInput={(e: any) => volUpdated(e.target.value)}
                        min="0"
                        max="15"
                    />
                    <button class="text-app-text-secondary pt-2" onClick={(e) => {
                        setMobileDialogOpen(false);
                        e.stopPropagation()
                    }
                    }>
                        Close
                    </button>
                </div>
            </Modal>) : null}
        </div>
    );
};

export default () => {
    return (
        <div>
            <PlaybackBar/>
        </div>
    );
};
