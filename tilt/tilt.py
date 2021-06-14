import json
import logging
import queue
from datetime import datetime
from .sign import sign_data

import requests
from beacontools import BeaconScanner, IBeaconAdvertisement
from requests.exceptions import HTTPError

BLACK_TILT = "a495bb30-c5b1-4b44-b512-1370f02d74de"
TIME_BETWEEN_PACKETS = 5 * 60
MIN_TEMP = 0
MAX_TEMP = 185
MIN_GRAVITY = 990
MAX_GRAVITY = 1120

tilt_queue = queue.Queue()
logger = logging.getLogger()
last_packet_sent = datetime.utcnow()
headers = {'Content-type': 'application/json'}


def start_scanner(url, name, private_key):
    logger.info("Starting scanner")
    scanner = BeaconScanner(_beacon_callback, packet_filter=IBeaconAdvertisement)
    scanner.start()
    logger.info("Scanner started")

    while True:
        data = tilt_queue.get()
        timestamp = data['time'].strftime("%m/%d/%Y, %H:%M:%S:%f")
        celsius = round((data['temp'] - 32) * 5.0 / 9.0)
        gravity = data['gravity'] * .001

        logger.info("Beer %s, timestamp %s, temp F %d, temp C %1.2f, gravity %1.4f" %
                    (name,
                     timestamp,
                     data['temp'],
                     celsius,
                     gravity
                    ))

        payload = dict()

        payload['name'] = name
        payload['timestamp'] = timestamp
        payload['fahrenheit'] = data['temp']
        payload['celsius'] = celsius
        payload['gravity'] = gravity
        payload['signature'] = sign_data(private_key, json.dumps(payload).encode('utf-8')).decode("utf-8")

        payload_json = json.dumps(payload)

        logger.debug("Sending to %s with headers %s the payload %s" % (url, headers, payload_json))

        try:
            response = requests.post(url, data=payload_json, headers=headers, timeout=10)
            response.raise_for_status()
        except HTTPError as http_err:
            logger.exception(http_err)
        except Exception as err:
            logger.exception(err)
        else:
            logger.info("Payload delivered!")


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
