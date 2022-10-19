import { ErrorCallback, OpenOptions, SerialPortStream } from '@serialport/stream';
import { MockBindingInterface } from '@serialport/binding-mock';
export declare type SerialPortMockOpenOptions = Omit<OpenOptions<MockBindingInterface>, 'binding'>;
export declare class SerialPortMock extends SerialPortStream<MockBindingInterface> {
    static list: () => Promise<import("@serialport/bindings-cpp").PortInfo[]>;
    static readonly binding: MockBindingInterface;
    constructor(options: SerialPortMockOpenOptions, openCallback?: ErrorCallback);
}
