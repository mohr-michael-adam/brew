import logging
import queue
from datetime import datetime
from beacontools import BeaconScanner, IBeaconAdvertisement

BLACK_TILT = "a495bb30-c5b1-4b44-b512-1370f02d74de"
TIME_BETWEEN_PACKETS = 5 * 60
MIN_TEMP = 0
MAX_TEMP = 185
MIN_GRAVITY = 990
MAX_GRAVITY = 1120

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

        logger.info("Timestamp %s, temp F %d, temp C %1.2f, gravity %1.4f" %
                    (data['time'].strftime("%m/%d/%Y, %H:%M:%S:%f"),
                     data['temp'],
                     round((data['temp'] - 32) * 5.0/9.0),
                     data['gravity'] * .001)
                    )


def _beacon_callback(bt_addr, rssi, packet, additional_info):
    uuid = packet.uuid
    major = packet.major
    minor = packet.minor

    # Data format documented here - https://kvurd.com/blog/tilt-hydrometer-ibeacon-data-format/

    data = dict()

    data['time'] = datetime.utcnow()
    data['temp'] = major
    data['gravity'] = minor

    global last_packet_sent

    delta = (data['time'] - last_packet_sent).total_seconds()

    logger.debug("UUID %s, major %d, minor %d, time since last packet %d" % (uuid, major, minor, delta))

    # Valid values documented here - https://tilthydrometer.com/pages/faqs

    if data['temp'] < MIN_TEMP or data['temp'] > MAX_TEMP:
        logger.debug('Invalid temp %d' % data['temp'])
        return

    if data['gravity'] < MIN_GRAVITY or data['gravity'] > MAX_GRAVITY:
        logger.debug('Invalid gravity %d' % data['gravity'])
        return

    if delta > TIME_BETWEEN_PACKETS and packet.uuid == BLACK_TILT:
        tilt_queue.put_nowait(data)
        last_packet_sent = data['time']
