import time
from beacontools import BeaconScanner, IBeaconAdvertisement


class Scanner:
    logger = None

    def __init__(self, logger):
        Scanner.logger = logger

    def start_scanner(self):
        scanner = BeaconScanner(_beacon_callback, packet_filter=IBeaconAdvertisement)
        scanner.start()
        Scanner.logger.warning("Scanning started")


def _beacon_callback(bt_addr, rssi, packet, additional_info):
    uuid = packet.uuid
    major = packet.major
    minor = packet.minor

    Scanner.logger.warning("UUID %s, major %d, minor %d" % (uuid, major, minor))