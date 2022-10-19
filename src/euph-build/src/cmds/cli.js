import buildCLI from "../jobs/build-cli.js";
import buildFrontend from "../jobs/build-frontend.js";
import prepareFS from "../jobs/prepare-fs.js";

import {
  initializeJobs,
  loadSetingsAndExecuteJobs,
  registerJob,
} from "../jobs/job-manager.js";

export default {
  command: "cli <action>",
  describe: "Actions related to CLI target",
  handler: async (parsed) => {
    switch (parsed["action"]) {
      case "build": {
        if (!parsed["skipFrontend"]) {
          registerJob(buildFrontend);
        }
        registerJob(prepareFS);
        registerJob(buildCLI);
        initializeJobs();
        await loadSetingsAndExecuteJobs(parsed);
        return;
      }
    }
  },
  builder: {
    action: {
      demand: true,
      choices: ["build", "run"],
      default: "build",
    },
    "skip-frontend": {
      demand: false,
      default: false,
      type: "boolean",
      describe: "Skips frontend build step",
    },
  },
};
