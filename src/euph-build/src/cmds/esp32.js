import buildESP32 from "../jobs/build-esp32.js";
import flashESP32 from "../jobs/flash-esp32.js";
import buildFrontend from "../jobs/build-frontend.js";
import prepareFS from "../jobs/prepare-fs.js";

import {
  initializeJobs,
  loadSetingsAndExecuteJobs,
  registerJob,
} from "../jobs/job-manager.js";

export default {
  command: "esp32 <action>",
  aliases: ["32"],
  describe: "Actions related to ESP32 target",
  handler: async (parsed) => {
    switch (parsed["action"]) {
      case "build": {
        if (!parsed["skipFrontend"]) {
          registerJob(buildFrontend);
        }
        registerJob(prepareFS);
        registerJob(buildESP32);
        initializeJobs();
        await loadSetingsAndExecuteJobs(parsed);
        return;
      }
      case "flash": {
        if (!parsed["skipFrontend"]) {
          registerJob(buildFrontend);
        }
        registerJob(prepareFS);
        registerJob(buildESP32);
        registerJob(flashESP32);
        initializeJobs();
        loadSetingsAndExecuteJobs(parsed);
        return;
      }
      case "monitor": {
      }
    }
  },
  builder: {
    action: {
      demand: true,
      choices: ["build", "flash", "monitor", "flash-remote"],
      default: "build",
    },
    "fs-only": {
      demand: false,
      default: false,
      type: "boolean",
      describe: "Only flashes the littlefs partition",
    },
    remote: {
      demand: false,
      default: null,
      describe: "Address of a euphonium instance, for OTA",
    },
    "port": {
      aliases: ['p'],
      demand: false,
      describe: 'Specify a serial port for flash. Will prompt if not defined.'
    },
    "skip-frontend": {
      demand: false,
      default: false,
      type: "boolean",
      describe: "Skips frontend build step",
    },
  },
};
