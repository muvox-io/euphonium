import { asyncExec } from "../utils/async-exec.js";
import serialport from "serialport";
import inquirer from "inquirer";

export const flashESP32App = async ({ incrementStep, config, args }) => {
  // get serial ports
  if (!args["port"]) {
    const portObjs = await serialport.SerialPort.list();
    const ports = portObjs.map(({ path }) => path);
    await inquirer.prompt([
      {
        type: "list",
        name: "serial",
        message: "Select serial port",
        choices: ports,
      },
    ]);
  }
  incrementStep();
  // await asyncExec("idf.py", ['flash'], { cwd: config.targetESP32AppPath });
};

const initJob = async ({ registerStep }) => {
  registerStep("Flashing ESP32 app");
};

export default {
  name: "FLASH",
  init: initJob,
  execute: flashESP32App,
};
