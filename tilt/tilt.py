import time
from beacontools import BeaconScanner, IBeaconAdvertisement


def start_scanner():
    scanner = BeaconScanner(_beacon_callback, packet_filter=IBeaconAdvertisement)
    scanner.start()
    print("scanning started")

    time.sleep(10000)


def _beacon_callback(bt_addr, rssi, packet, additional_info):
    uuid = packet.uuid
    major = packet.major
    minor = packet.minor

    print("UUID %s, major %d, minor %d" % (uuid, major, minor))

