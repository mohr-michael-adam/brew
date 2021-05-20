import argparse


def _get_args():
    parser = argparse.ArgumentParser(description='Read measurements from the Tilt hydrometer and push to a URL.')
    parser.add_argument('--min-temp', dest='min_temp', action='store_true',
                        help='The minimum temperature we will accept.')
    parser.add_argument('--max-temp', dest='min_temp', action='store_true',
                        help='The maximum temperature we will accept.')
    parser.add_argument('--min-gravity', dest='min_temp', action='store_true',
                        help='The minimum gravity we will accept.')
    parser.add_argument('--max-gravity', dest='min_temp', action='store_true',
                        help='The maximum gravity we will accept.')

    return parser.parse_args()


if __name__ == '__main__':
    args = _get_args()

    print("Not much here!")
