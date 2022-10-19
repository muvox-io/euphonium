"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.SerialPortMock = void 0;
const stream_1 = require("@serialport/stream");
const binding_mock_1 = require("@serialport/binding-mock");
class SerialPortMock extends stream_1.SerialPortStream {
    constructor(options, openCallback) {
        const opts = {
            binding: binding_mock_1.MockBinding,
            ...options,
        };
        super(opts, openCallback);
    }
}
exports.SerialPortMock = SerialPortMock;
SerialPortMock.list = binding_mock_1.MockBinding.list;
SerialPortMock.binding = binding_mock_1.MockBinding;
