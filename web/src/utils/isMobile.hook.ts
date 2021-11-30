import { useLayoutEffect, useState } from "preact/hooks";

function useIsMobile() {
    const [isMobile, setIsMobile] = useState(window.innerWidth < 768);
    useLayoutEffect(() => {
        function updateSize() {
            const newIsMobile = window.innerWidth < 768;
            if (newIsMobile != isMobile) {
                setIsMobile(newIsMobile);
            }
        }
        window.addEventListener('resize', updateSize);
        updateSize();
        return () => window.removeEventListener('resize', updateSize);
    }, []);
    return isMobile;
}

export default useIsMobile;