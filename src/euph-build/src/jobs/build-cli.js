import { log } from "../utils/log.js";
import { asyncExec } from "../utils/async-exec.js";

export const buildCLI = async ({ incrementStep, config }) => {
    // Make dir if it doesn't exist
    await asyncExec("mkdir", ["-p", config.outputDir]);
    incrementStep();

    await asyncExec("cmake", ["../src/targets/cli",  "-GNinja"], { cwd: config.outputDir });
    incrementStep();

    await asyncExec("ninja", [], { cwd: config.outputDir });
    incrementStep();
};

const initJob = async ({ registerStep }) => {
    registerStep("Create build dir");
    registerStep("Generate CMake files");
    registerStep("Build cmake");
};

export default {
    name: 'CLI',
    init: initJob,
    execute: buildCLI,
}