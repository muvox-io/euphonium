export const timestampToHumanDuration = (current: number, previous: number) => {
  const msPerMinute = 60 * 1000;
  const msPerHour = msPerMinute * 60;
  const msPerDay = msPerHour * 24;
  const msPerMonth = msPerDay * 30;
  const msPerYear = msPerDay * 365;

  var elapsed = current - previous;

  if (elapsed < msPerMinute) {
    return Math.floor(elapsed/1000) + 's ago';
  } else if (elapsed < msPerHour) {
    return Math.floor(elapsed / msPerMinute) + 'm ago';
  } else if (elapsed < msPerDay) {
    return Math.floor(elapsed / msPerHour) + 'h ago';
  } else if (elapsed < msPerMonth) {
    return Math.floor(elapsed / msPerDay) + 'd ago';
  } else if (elapsed < msPerYear) {
    return Math.floor(elapsed / msPerMonth) + 'mon ago';
  }

  return Math.floor(elapsed / msPerYear) + 'y ago';
}

