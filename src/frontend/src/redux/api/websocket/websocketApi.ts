export interface WebsocketMessage {
  type: string;
  data: any;
}

export interface NotificationMessagePayload {
  source: string;
  message: string;
  submessage: string;
  type: string;
}
