import time
from beacontools import BeaconScanner, IBeaconAdvertisement


class Scanner:
    def __init__(self, logger):
        self.logger = logger

    def start_scanner(self):
        scanner = BeaconScanner(_beacon_callback, packet_filter=IBeaconAdvertisement)
        scanner.start()
        self.logger.warning("Scanning started")

    @staticmethod
    def get_logger(self):
        return self.logger


def _beacon_callback(bt_addr, rssi, packet, additional_info):
    uuid = packet.uuid
    major = packet.major
    minor = packet.minor

    Scanner.get_logger("UUID %s, major %d, minor %d" % (uuid, major, minor))

