"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.SerialPort = void 0;
const stream_1 = require("@serialport/stream");
const bindings_cpp_1 = require("@serialport/bindings-cpp");
const DetectedBinding = (0, bindings_cpp_1.autoDetect)();
class SerialPort extends stream_1.SerialPortStream {
    constructor(options, openCallback) {
        const opts = {
            binding: DetectedBinding,
            ...options,
        };
        super(opts, openCallback);
    }
}
exports.SerialPort = SerialPort;
SerialPort.list = DetectedBinding.list;
SerialPort.binding = DetectedBinding;
