import logging
import queue
import time
from datetime import datetime
from beacontools import BeaconScanner, IBeaconAdvertisement

BLACK_TILT = "a495bb30-c5b1-4b44-b512-1370f02d74de"

tilt_queue = queue.Queue()

logger = logging.getLogger()


def start_scanner():
    logger.info("Starting scanner")
    scanner = BeaconScanner(_beacon_callback, packet_filter=IBeaconAdvertisement)
    scanner.start()
    logger.info("Scanner started")

    data = tilt_queue.get()

    logger.info("Timestamp %s, major %d, minor %d" % (data['time'].strftime("%m/%d/%Y, %H:%M:%S:%f"), data['major'], data['minor']))


def _beacon_callback(bt_addr, rssi, packet, additional_info):
    uuid = packet.uuid

    data = dict()

    data['time'] = datetime.utcnow()
    data['major'] = package.major
    data['minor'] = package.minor
        
    logger.info("UUID %s, major %d, minor %d" % (uuid, major, minor))

    if packet.uuid == BLACK_TILT:
        tilt_queue.put_nowait(data)

