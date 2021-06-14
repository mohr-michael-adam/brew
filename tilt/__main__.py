import argparse
import logging
import sys
import validators

from tilt import start_scanner

DEFAULT_LOG_FILE = 'tilt.log'
DEFAULT_LOG_LEVEL = 'WARNING'


class StreamToLogger(object):

    def __init__(self, logger, log_level=logging.INFO):
        self.logger = logger
        self.log_level = log_level
        self.linebuf = ''

    def write(self, buf):
        for line in buf.rstrip().splitlines():
            self.logger.log(self.log_level, line.rstrip())


def _config_logger(args):
    log_file = DEFAULT_LOG_FILE
    log_level = DEFAULT_LOG_LEVEL

    if args.log_file:
        log_file = args.log_file

    if args.log_level:
        log_level = args.log_level

    logging.basicConfig(filename=log_file, level=log_level, format='%(asctime)s - %(levelname)s - %(message)s',
                        datefmt='%d-%b-%y %H:%M:%S')

    stderr_logger = logging.getLogger('STDERR')
    sl = StreamToLogger(stderr_logger, logging.ERROR)
    sys.stderr = sl


def _get_args():
    parser = argparse.ArgumentParser(description='Read measurements from the Tilt hydrometer and push to a URL.')
    parser.add_argument('--log-file', dest='log_file', help='The log file')
    parser.add_argument('--log-level', dest='log_level', help='The log level')
    parser.add_argument('url', dest='url', help='URL to send payload to')

    return parser.parse_args()


if __name__ == '__main__':
    args = _get_args()
    _config_logger(args)

    if not validators.url(args.url):
        logging.getLogger().error('Invalid URL: %s' % args.url)
        sys.exit(1)

    start_scanner(args.url)
