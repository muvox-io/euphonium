export default {
    command: "package <action>",
    aliases: ["pkg"],
    describe: "Actions related to euphonium packaging",
    handler: (parsed) => console.log("your handler goes here", parsed),
    builder: {
      action: {
        demand: true,
        choices: ["package", "install"],
        default: "package",
      },
    },
  };