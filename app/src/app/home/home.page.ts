/* eslint-disable eqeqeq */
import { ChangeDetectorRef, Component, OnInit } from '@angular/core';
import { Browser } from '@capacitor/browser';
import { Zeroconf } from '@ionic-native/zeroconf';

@Component({
  selector: 'app-home',
  templateUrl: 'home.page.html',
  styleUrls: ['home.page.scss'],
})
export class HomePage implements OnInit {
  devices = [];
  filteredDevices = [];
  loading = false;

  constructor(private cdr: ChangeDetectorRef) {}

  ngOnInit() {
    Zeroconf.watchAddressFamily = 'ipv4';
    this.scan();
  }

  async refresh(event) {
    this.devices = [];
    await Zeroconf.reInit();
    setTimeout(() => {
      this.scan(event);
    }, 200);
  }

  async scan(event?) {
    this.loading = true;

    Zeroconf.watch('_http._tcp.', 'local.').subscribe((result) => {
      console.log('Zeroconf', result);

      if (result.service?.ipv4Addresses[0]) {
        const device = this.devices.find(
          (_) => _.service?.name == result.service?.name
        );
        if (device) {
          this.devices.map((_) => {
            if (
              _.service?.name == result.service?.name &&
              result.service?.ipv4Addresses[0]
            ) {
              return { ...result };
            }
            return _;
          });
        } else {
          this.devices.push(result);
        }
      }

      this.loading = false;
      this.cdr.detectChanges();

      event?.target?.complete();
    });
  }

  async openDevice(device) {
    const url = `http://${device.service?.ipv4Addresses[0]}`;
    if (url) {
      await Browser.open({ url });
    }
  }
}
