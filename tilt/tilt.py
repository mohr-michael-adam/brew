import logging
import queue
from datetime import datetime
from beacontools import BeaconScanner, IBeaconAdvertisement

BLACK_TILT = "a495bb30-c5b1-4b44-b512-1370f02d74de"
TIME_BETWEEN_PACKETS = 5 * 60

tilt_queue = queue.Queue()
logger = logging.getLogger()
last_packet_sent = datetime.utcnow()


def start_scanner():
    logger.info("Starting scanner")
    scanner = BeaconScanner(_beacon_callback, packet_filter=IBeaconAdvertisement)
    scanner.start()
    logger.info("Scanner started")

    while True:
        data = tilt_queue.get()

        logger.info("Timestamp %s, major %d, minor %d" % (data['time'].strftime("%m/%d/%Y, %H:%M:%S:%f"), data['major'], data['minor']))


def _beacon_callback(bt_addr, rssi, packet, additional_info):
    uuid = packet.uuid
    major = packet.major
    minor = packet.minor

    data = dict()

    data['time'] = datetime.utcnow()
    data['major'] = major
    data['minor'] = minor

    global last_packet_sent

    delta = (data['time'] - last_packet_sent).total_seconds()

    logger.info("UUID %s, major %d, minor %d, time since last packet %d" % (uuid, major, minor, delta))

    if delta > TIME_BETWEEN_PACKETS and packet.uuid == BLACK_TILT:
        tilt_queue.put_nowait(data)
        last_packet_sent = data['time']
