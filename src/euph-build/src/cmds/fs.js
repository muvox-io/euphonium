export default {
    command: "fs <action>",
    describe: "Actions related to internal filesystem of euphonium",
    handler: (parsed) => console.log("your handler goes here", parsed),
    builder: {
      action: {
        demand: true,
        choices: ["build", "dev", "deps"],
        default: "build",
      },
    },
  };