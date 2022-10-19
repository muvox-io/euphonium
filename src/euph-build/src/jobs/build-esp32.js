import { asyncExec } from "../utils/async-exec.js";

export const buildESP32App = async ({ incrementStep, config }) => {
    await asyncExec("idf.py", ['build'], { cwd: config.targetESP32AppPath });
    incrementStep();
};

const initJob = async ({ registerStep }) => {
    registerStep("Building ESP32 app");
};

export default {
    name: 'ESP32',
    init: initJob,
    execute: buildESP32App,
}