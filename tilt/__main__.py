import argparse
import logging
from tilt import start_scanner

DEFAULT_LOG_FILE = 'tilt.log'
DEFAULT_LOG_LEVEL = 'WARNING'


def _get_config():

    parser = argparse.ArgumentParser(description='Read measurements from the Tilt hydrometer and push to a URL.')
    parser.add_argument('--log-file', dest='log_file', help='The log file')
    parser.add_argument('--log-level', dest='log_level', help='The log level')

    parsed_args = parser.parse_args()

    log_file = DEFAULT_LOG_FILE
    log_level = DEFAULT_LOG_LEVEL

    if parsed_args.log_file:
        log_file = parsed_args.log_file

    if parsed_args.log_level:
        log_level = parsed_args.log_level

    logging.basicConfig(filename=log_file, level=log_level, format='%(asctime)s - %(levelname)s - %(message)s',
                        datefmt='%d-%b-%y %H:%M:%S')
    return logging.getLogger()


if __name__ == '__main__':
    logger = _get_config()

    logger.debug("Not much here!")

    start_scanner()
