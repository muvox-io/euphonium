import { asyncExec } from "../utils/async-exec.js";

export const frontendRunDev = async ({ incrementStep, config }) => {
    await asyncExec("yarn", ["dev"], { cwd: config.webPath, showOutput: true });
    incrementStep();
};

const initJob = async ({ registerStep }) => {
    registerStep("Running web in dev mode");
};

export default {
    name: 'WEB',
    init: initJob,
    execute: frontendRunDev,
}