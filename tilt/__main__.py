import argparse
import logging
from tilt import start_scanner

DEFAULT_LOG_FILE = 'tilt.log'
DEFAULT_LOG_LEVEL = 'WARNING'


def _get_logger(args):
    log_file = DEFAULT_LOG_FILE
    log_level = DEFAULT_LOG_LEVEL

    if args.log_file:
        log_file = args.log_file

    if args.log_level:
        log_level = args.log_level

    logging.basicConfig(filename=log_file, level=log_level, format='%(asctime)s - %(levelname)s - %(message)s',
                        datefmt='%d-%b-%y %H:%M:%S')

    return logging.getLogger()


def _get_args():

    parser = argparse.ArgumentParser(description='Read measurements from the Tilt hydrometer and push to a URL.')
    parser.add_argument('--log-file', dest='log_file', help='The log file')
    parser.add_argument('--log-level', dest='log_level', help='The log level')

    return parser.parse_args()


if __name__ == '__main__':
    args = _get_args()
    logger = _get_logger(args)

    logger.warning("Not much here!")

    start_scanner()
