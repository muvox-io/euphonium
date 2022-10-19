import { asyncExec } from "../utils/async-exec.js";

export const prepareFS = async ({ incrementStep, config }) => {
    await asyncExec("rm", ['-rf', 'fs-tmp'], { cwd: config.outputDir });
    
    // Copy base fs
    await asyncExec("/bin/cp", ['-rf', config.fsPath, config.outputDir + '/fs-tmp']);

    // Apply user's overrides
    await asyncExec("/bin/cp", ['-rf', config.fsDefaultsPath + '/', config.outputDir + '/fs-tmp']);

    // Copy built web-ui into fs structure
    await asyncExec("/bin/cp", ['-rf', config.webPath + '/dist', config.outputDir + '/fs-tmp/pkgs/frontend']);
    incrementStep();
};

const initJob = async ({ registerStep }) => {
    registerStep("Generating FS structure");
};

export default {
    name: 'FS',
    init: initJob,
    execute: prepareFS,
}