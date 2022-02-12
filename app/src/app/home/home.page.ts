/* eslint-disable eqeqeq */
import { ChangeDetectorRef, Component, OnInit } from '@angular/core';
import { Browser } from '@capacitor/browser';
import { Zeroconf } from '@ionic-native/zeroconf';
import { Storage } from '@capacitor/storage';


@Component({
  selector: 'app-home',
  templateUrl: 'home.page.html',
  styleUrls: ['home.page.scss'],
})
export class HomePage implements OnInit {
  devices: any = [];
  filteredDevices = [];
  favorites = [];
  loading = false;
  debug = false;

  constructor(private cdr: ChangeDetectorRef) {}

  async ngOnInit() {
    Zeroconf.watchAddressFamily = 'ipv4';
    this.loading = false;
    // this.scan();
    const { value } = await Storage.get({ key: 'favorites' });
    if(value) {
      this.favorites = JSON.parse(value);
    }
    console.log('value', this.favorites);

  }

  async clearFavorites() {
    this.favorites = [];
    await Storage.remove({ key: 'favorites' });
  }

  isInWishlist(device) {
    return this.favorites?.includes(device);
  }

  async favorite(event, device) {
    event.preventDefault();
    event.stopPropagation();
    console.log('favorite', device);
    this.favorites.push(device);
    await Storage.set({
      key: 'favorites',
      value: JSON.stringify(this.favorites),
    });
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

    Zeroconf.watch('_euphonium._tcp.', 'local.').subscribe((result) => {
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
