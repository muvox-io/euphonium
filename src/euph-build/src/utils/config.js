import fs from "fs";

// Method that reads "euph-build.json" and returns the config object
export const getConfig = async () => {
  // Check if "euph-build.json" exists
  if (!fs.existsSync("euph-build.json")) {
    throw new Error("euph-build.json not found. Are you in the right directory?");
  }

  const config = fs.readFileSync("euph-build.json", "utf8");
  return JSON.parse(config);
};
