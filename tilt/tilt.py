import logging
import queue
import time
from beacontools import BeaconScanner, IBeaconAdvertisement

BLACK_TILT = "a495bb30-c5b1-4b44-b512-1370f02d74de"

tilt_queue = queue.Queue()

logger = logging.getLogger()


def start_scanner():
    logger.info("Starting scanner")
    scanner = BeaconScanner(_beacon_callback, packet_filter=IBeaconAdvertisement)
    scanner.start()
    logger.info("Scanner started")


def _beacon_callback(bt_addr, rssi, packet, additional_info):
    uuid = packet.uuid
    major = packet.major
    minor = packet.minor

    logger.info("UUID %s, major %d, minor %d" % (uuid, major, minor))
