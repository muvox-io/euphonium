import progress from "cli-progress";
import colors from "ansi-colors";
import { log } from "../utils/log.js";
import { getConfig } from "../utils/config.js";

// holds reference to all registered build steps
let jobs = [];
let steps = [];
let currentStep = 0;
let currentJob = 0;

// register a new build step
export const registerJob = async ({ name, init, execute }) => {
  const job = {
    name,
    init,
    execute,
  };
  jobs.push(job);
};

/**
 * Registers a single step of a build process
 * @param name Name of the step, used for logging
 */
export const registerStep = async (name) => {
  steps.push(name);
};

/**
 * Executes initialization of all the registered jobs
 */
export const initializeJobs = async () => {
  jobs.forEach((job) => job.init({ registerStep }));
};

/**
 * Increpents the step counter and updates the progress bar
 */
export const incrementStep = async () => {
  currentStep++;
  updateStep();
};

export const updateStep = async () => {
  if (steps[currentStep]) {
    console.log(
      colors.grey("[" + (currentStep + 1) + "/" + steps.length + "]"),
      colors.bgGreen(jobs[currentJob].name),
      colors.reset(steps[currentStep])
    );
  }
};

export const executeJobs = async ({config, args}) => {
  updateStep();
  for (const job of jobs) {
    try {
      await job.execute({
        incrementStep,
        config,
        args
      });
      currentJob++;
    } catch (e) {
      if (e?.code) {
        log("Job failed with code " + e.code);
        log(colors.bgRed("OUTPUT"))
        log("");
        console.log(colors.bgRedBright(e?.output));
        log("");
        log(colors.bgRed("ERROR") + colors.reset(" Build terminated due to error. See above for details."));
        log(colors.bgBlue("COMMAND") + colors.grey(" " + e.cmd))
      }

      process.exit(1);
      return;
    }
  }
};

export const loadSetingsAndExecuteJobs = async (args) => {
  const config = await getConfig();
  executeJobs({ config, args });
}