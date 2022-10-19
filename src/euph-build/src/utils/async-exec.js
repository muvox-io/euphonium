import { spawn } from "child_process";
import { promisify } from "util";

export const asyncExec = async (command, args, config) => {
  // const res = await promiseExec(command, { cwd: config?.cwd });
  // return res;
  return new Promise((resolve, reject) => {
    let child = spawn(command, args, { cwd: config?.cwd });
    let output = "";

    child.stdout.setEncoding("utf8");
    child.stdout.on("data", function (data) {
      if (config?.showOutput) {
        console.log(data)
      }
      output += data;
    });

    child.stderr.setEncoding("utf8");
    child.stderr.on("data", function (data) {
      if (config?.showOutput) {
        console.log(data)
      }
      output += data;
    });

    child.on("close", function (code) {
      if (code != 0) {
        reject({
            code: code,
            output: output,
            cmd: command + " " + args.join(" "),
        });
      }
      resolve({
        code,
        output,
      });
    });
  });
};
