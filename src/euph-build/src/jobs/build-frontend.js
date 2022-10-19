import { log } from "../utils/log.js";
import { asyncExec } from "../utils/async-exec.js";

export const buildFrontend = async ({ incrementStep, config }) => {
    await asyncExec("yarn", [], { cwd: config.webPath });
    incrementStep();

    await asyncExec("yarn", ["build"], { cwd: config.webPath});
    incrementStep();
};

const initJob = async ({ registerStep }) => {
    registerStep("Installing dependencies");
    registerStep("Building frontend");
};

export default {
    name: 'WEB',
    init: initJob,
    execute: buildFrontend,
}