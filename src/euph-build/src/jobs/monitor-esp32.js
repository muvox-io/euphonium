import { asyncExec } from "../utils/async-exec.js";

export const monitorESP32App = async ({ incrementStep, config }) => {
    await asyncExec("idf.py", ['monitor'], { cwd: config.targetESP32AppPath });
    incrementStep();
};

const initJob = async ({ registerStep }) => {
    registerStep("Monitoring ESP32 app");
};

export default {
    name: 'Monitor ESP32 Target',
    init: initJob,
    execute: monitorESP32App,
}