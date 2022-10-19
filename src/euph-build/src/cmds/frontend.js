import devFrontend from "../jobs/dev-frontend.js";
import { loadSetingsAndExecuteJobs, registerJob } from "../jobs/job-manager.js";

export default {
  command: "frontend <action>",
  describe: "Actions related to building frontend",
  handler: async (parsed) => {
    switch (parsed["action"]) {
      case "dev": {
        registerJob(devFrontend);
        loadSetingsAndExecuteJobs(parsed);
      }
    }
  },
  builder: {
    action: {
      demand: true,
      choices: ["build", "dev", "deps"],
      default: "build",
    },
  },
};
