import { ErrorCallback, SerialPortStream, StreamOptions } from '@serialport/stream';
import { AutoDetectTypes, OpenOptionsFromBinding } from '@serialport/bindings-cpp';
export declare type SerialPortOpenOptions<T extends AutoDetectTypes> = Omit<StreamOptions<T>, 'binding'> & OpenOptionsFromBinding<T>;
export declare class SerialPort<T extends AutoDetectTypes = AutoDetectTypes> extends SerialPortStream<T> {
    static list: () => Promise<import("@serialport/bindings-cpp").PortInfo[]>;
    static readonly binding: AutoDetectTypes;
    constructor(options: SerialPortOpenOptions<T>, openCallback?: ErrorCallback);
}
