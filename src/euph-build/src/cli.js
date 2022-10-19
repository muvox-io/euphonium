

import yargs from "yargs/yargs";
import esp32Cmd from './cmds/esp32.js';
import cliCmd from './cmds/cli.js';
import frontendCmd from './cmds/frontend.js';
import fsCmd from './cmds/fs.js';
import packageCmd from './cmds/package.js';

const yargsObj = yargs(process.argv.slice(2));


yargsObj
  .scriptName("euph-build")
  .command(esp32Cmd)
  .command(cliCmd)
  .command(frontendCmd)
  .command(fsCmd)
  .command(packageCmd)
  .demandCommand()
  .help("h").argv;
