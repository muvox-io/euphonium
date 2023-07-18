import { ConfigurationGraph } from "../../../api/euphonium/plugins/models";
import { FieldProps } from "./FieldProps";

// Constants for the width and height of the graph
const HEIGHT = 300;
const WIDTH = 500;
const MARGIN = 50; // Margin to accommodate labels

// Define an array of line positions in terms of percentage
const LINE_POSITIONS = [0, 25, 50, 75, 100];

export default function Graph({ field }: FieldProps<ConfigurationGraph>) {
  const data: number[] = field.data;
  const maxValue = field.max_value;

  // Generate points for the polyline
  let pointsMapped = data.map((value, index) => {
    let pos =  (index / (data.length - 1));
    if(field.reverse_data) {
      pos = 1 - pos;
    }
    let x = pos * (WIDTH - MARGIN) + MARGIN; // Add margin for the labels
   
    const y = HEIGHT - (value / maxValue) * HEIGHT;
    return `${x},${y}`;
  });
  

  let points = pointsMapped.join(" ");

  return (
    <svg width={WIDTH + MARGIN} height={HEIGHT + MARGIN}>
      <polyline points={points} fill="none" stroke="white" strokeWidth="2" />
      <line
        x1={MARGIN}
        y1={HEIGHT}
        x2={WIDTH + MARGIN}
        y2={HEIGHT}
        stroke="white"
      />
      <line x1={MARGIN} y1="0" x2={MARGIN} y2={HEIGHT} stroke="white" />

      {LINE_POSITIONS.map((position) => (
        <>
          <line
            x1={MARGIN}
            y1={HEIGHT - (position / 100) * HEIGHT}
            x2={WIDTH + MARGIN}
            y2={HEIGHT - (position / 100) * HEIGHT}
            stroke="lightgray"
          />
          <text
            x="0"
            y={HEIGHT - (position / 100) * HEIGHT}
            dx="2"
            dy="2"
            fill="white"
            alignmentBaseline={position === 100 ? "hanging" : "baseline"}
          >
            {`${position}%`}
          </text>
        </>
      ))}
    </svg>
  );
}
